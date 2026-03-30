// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "gps.h"

#define MOD "modules/gps/gps.h"

int Gps::push(const Task &task) {
  return queue_.push(task);
}

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
      return 1;
    }
  }
  
  if (lap_active_) {
    if (start_line_trigger_ == trigger_status::Idle) {
      float current_lat = gps_->location.lat();  
      float current_lng = gps_->location.lat();  
    }
  }
  
  Task active; 
  int res = queue_.pop(active);
  if (res == 0) {
    if (active.target_ == module::Gps) {

    } else if (active.target_ == module::All) {
      switch (active.type_)
      {
      case task::AllTrackLoaded:
        #ifdef DEBUG
        if (logger_ != nullptr) {
          logger_->debug("GPS received track loaded sig");
        }
        #endif
        lap_active_ = true;
        break;
      
      default:
        break;
      }
    }
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

  output.num_fix_ = gps_->sentencesWithFix();

  return output;
}

#undef MOD
