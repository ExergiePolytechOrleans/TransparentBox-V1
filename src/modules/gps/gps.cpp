// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "gps.h"
#include "math.h"

#include "data/track_store.h"

#define MOD "modules/gps/gps.h"

int Gps::push(const Task &task) { return queue_.push(task); }

Gps::Gps(HardwareSerial *data_stream)
    : gps_(nullptr), data_stream_(data_stream), logger_(nullptr) {
  gps_ = new TinyGPSPlus();
}

Gps::Gps(HardwareSerial *data_stream, SystemLogger *logger)
    : gps_(nullptr), data_stream_(data_stream), logger_(logger) {
  gps_ = new TinyGPSPlus();
}

Gps::~Gps() {
  data_stream_ = nullptr;
  delete gps_;
  gps_ = nullptr;
}

int Gps::init() {
  data_stream_->begin(9600);
  return 0;
}

int Gps::loop(unsigned long timeout_ms) {
  unsigned long timeout = millis() + timeout_ms;
  bool timed_out = false;

  while (data_stream_->available() > 0) {
    if (gps_->encode(data_stream_->read())) {
      gpsGlobalWrite(this->getData());
      uint32_t current_fix_value = gps_->sentencesWithFix();
      if (last_fix_value_ == 0 && current_fix_value > 0) {
        router::send(module::Lcd, task::DisplayMsgGpsFix, 2000);
        router::send(module::Config, task::ConfigTrackDetect);
      }
      last_fix_value_ = current_fix_value;
    }

    if (millis() > timeout) {
      timed_out = true;
      break;
    }
  }

  Task active;
  int res = queue_.pop(active);
  if (res == 0) {
    if (active.target_ == module::Gps) {

    } else if (active.target_ == module::All) {
      switch (active.type_) {
      case task::AllTrackLoaded: {
#ifdef DEBUG
        if (logger_ != nullptr) {
          logger_->debug("GPS received track loaded sig");
        }
#endif
        GlobalTrackData track;
        trackGlobalRead(track);
        lap_active_ = track.loaded_;
        start_line_trigger_ = trigger_status::Idle;
        gpsTriggerGlobalWrite(start_line_trigger_);
        arm_sign_ = 0;
        if (!track.loaded_) {
          break;
        }
        track_point_a_ = track.root_.point_a_;
        track_point_b_ = track.root_.point_b_;
        track_vec_b_ = eqRectProjection(track_point_b_, track_point_a_);
        track_vec_center_ = track.center_;
        start_line_length_ = vecMod(track_vec_b_);
        start_line_sq_dist_ = track.circle_radius_sq_;
        break;
      }

      default:
        break;
      }
    }
  }

  if (lap_active_ && millis() - last_check_ > check_interval_) {
    unsigned long now = millis();
    GpsData gps;
    gpsGlobalRead(gps);
    LatLng vehicle_position = {gps.lat_.value_, gps.lng_.value_};

    Vec2 vehicle_pos_vec = eqRectProjection(vehicle_position, track_point_a_);
    float center_dist_sq = vec2SqDist(track_vec_center_, vehicle_pos_vec);
    float cross = vec2Cross(track_vec_b_, vehicle_pos_vec);
    int sign = 0;
    if (cross < 0) {
      sign = -1;
    } else if (cross > 0) {
      sign = 1;
    }

    switch (start_line_trigger_) {
    case trigger_status::Idle: {
      if (center_dist_sq < start_line_sq_dist_) {
        start_line_trigger_ = trigger_status::Armed;
        gpsTriggerGlobalWrite(start_line_trigger_);
        arm_sign_ = sign;
        state_changed_at_ = now;
      }
      break;
    };

    case trigger_status::Armed: {
      if (center_dist_sq > start_line_sq_dist_) {
        start_line_trigger_ = trigger_status::Idle;
        gpsTriggerGlobalWrite(start_line_trigger_);
        arm_sign_ = 0;
        state_changed_at_ = 0;
      } else if ((sign == -1 && arm_sign_ == 1) || (sign == 1 && arm_sign_ == -1)) {
        start_line_trigger_ = trigger_status::Trigd;
        gpsTriggerGlobalWrite(start_line_trigger_);
        arm_sign_ = 0;
        state_changed_at_ = now;
        router::send(module::All, task::AllStartLineTriggered);
      }
      break;
    };

    case trigger_status::Trigd: {
      if (center_dist_sq > start_line_sq_dist_) {
        start_line_trigger_ = trigger_status::Idle;
        gpsTriggerGlobalWrite(start_line_trigger_);
        arm_sign_ = 0;
        state_changed_at_ = now;
      }
      break;
    };

    default:
      break;
    }
    last_check_ = now;
  }
  return 0;
}

GpsData Gps::getData() {
  GpsData output;

  output.altitude_.age_ = gps_->altitude.age();
  output.altitude_.valid_ = gps_->altitude.isValid();
  output.altitude_.value_ = gps_->altitude.meters();

  output.lat_.age_ = gps_->location.age();
  output.lat_.valid_ = gps_->location.isValid();
  output.lat_.value_ = gps_->location.lat();

  output.lng_.age_ = gps_->location.age();
  output.lng_.valid_ = gps_->location.isValid();
  output.lng_.value_ = gps_->location.lng();

  output.speed_.age_ = gps_->speed.age();
  output.speed_.valid_ = gps_->speed.isValid();
  output.speed_.value_ = gps_->speed.kmph();

  output.course_.age_ = gps_->course.age();
  output.course_.valid_ = gps_->course.isValid();
  output.course_.value_ = gps_->course.deg();

  output.time_ = gps_->time.value();
  output.time_write_time_ = millis() - gps_->time.age();

  output.num_fix_ = gps_->sentencesWithFix();
  gps_->time.minute();

  return output;
}

#undef MOD
