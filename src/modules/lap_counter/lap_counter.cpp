// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "lap_counter.h"
#include "base/router.h"
#include "custom_types.h"
#include "data/general_store.h"
#include "data/gps_store.h"
#include "data/track_store.h"

int LapCounter::push(const Task &task) { return queue_.push(task); }

LapCounter::LapCounter() : logger_(nullptr) {};

LapCounter::LapCounter(SystemLogger *logger) : logger_(logger) {};

LapCounter::~LapCounter() {}

int LapCounter::init() {
  counting_ = false;
  count_ = false;
  return 0;
}

int LapCounter::loop() {
  Task active_task;
  int res = queue_.pop(active_task);

  if (res == 0) {
    if (active_task.target_ == module::LapCounter) {

    } else if (active_task.target_ == module::All) {
      switch (active_task.type_) {

      case task::AllStartLineTriggered: {
        GpsData gps;
        gpsGlobalRead(gps);

        uint32_t base_cs = hhmmsscc_to_cs(gps.time_);

        uint32_t elapsed_cs = (millis() - gps.time_write_time_) / 10;

        uint32_t time_cs = base_cs + elapsed_cs;

        if (!counting_) {
          counting_ = true;
          last_trigger_time_ = time_cs;

          router::send(module::Lcd, task::DisplayMsgLapCounterStart, 1000);

        } else {
          uint32_t lap_time = time_cs - last_trigger_time_;

          lap_times_idx_ = (lap_times_idx_ + 1) & 63;

          lap_times_[lap_times_idx_] = lap_time;
          count_++;
          lapCountGlobalWrite(count_);

          last_trigger_time_ = time_cs;

          lastLapTimeGlobalWrite(lap_time);

          router::send(module::Lcd, task::DisplayMsgLapCounterLapTime, 1000);
        }

        break;
      }

      case task::AllGpsFixOk: {
        average_enabled_ = true;
#ifdef DEBUG
        if (logger_ != nullptr) {
          logger_->debug("Enabled average counter");
        }
#endif
        break;
      }

      default:
        break;
      }
    }
  }

  if (millis() - last_average_time_ > average_loop_time_ && average_enabled_) {
    GpsData gps;
    gpsGlobalRead(gps);
    unsigned long now = millis();
    float dt = (now - last_average_time_) / 1000.0f;

    continuous_time_sum_ += dt;
    if (last_average_time_ == 0) {
      continuous_speed_sum_ += gps.speed_.value_ * dt;
    } else {
      continuous_speed_sum_ +=
          (gps.speed_.value_ + previous_speed_) * 0.5f * dt;
    }
    previous_speed_ = gps.speed_.value_;

    speedAvgGlobalWrite(continuous_speed_sum_ / continuous_time_sum_);

    last_average_time_ = now;
  }

  return 0;
}