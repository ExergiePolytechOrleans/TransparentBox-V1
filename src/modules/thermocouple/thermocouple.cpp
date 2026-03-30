// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "thermocouple.h"
#include "base/router.h"
#include "data/general_store.h"

int Thermocouple::push(const Task &task) { return queue_.push(task); }

Thermocouple::Thermocouple() : logger_(nullptr) {}

Thermocouple::Thermocouple(SystemLogger *logger) : logger_(logger) {}

Thermocouple::~Thermocouple() {}

int Thermocouple::init() {
  thermocouple_ = new MAX6675(THERMO_SCK, THERMO_CS, THERMO_SO);
  VehicleConfig config;
  configGlobalRead(config);
  low_ = config.teng_low_;
  high_ = config.teng_high_;
  return 0;
}

int Thermocouple::loop(unsigned long timeout_ms) {
  (void)timeout_ms;

  if (millis() > last_read_at_ + update_interval_) {
    temperature_ = thermocouple_->readCelsius();
    tengGlobalWrite(temperature_);
    last_read_at_ = millis();
    if (temperature_ > high_) {
      if (warning_sent_at_ == 0 ||
          millis() > warning_sent_at_ + warning_timeout_) {
        router::send(module::Lcd, task::DisplayMsgEngineTempHigh, 2000);
        warning_sent_at_ = millis();
      }
    } else if (temperature_ < low_) {
      if (warning_sent_at_ == 0 ||
          millis() > warning_sent_at_ + warning_timeout_) {
        router::send(module::Lcd, task::DisplayMsgEngineTempLow, 2000);
        warning_sent_at_ = millis();
      }
    }

    Task active_task;
    int res = queue_.pop(active_task);
    if (res == 0) {
      if (active_task.target_ == module::Thermocouple) {

      } else if (active_task.target_ == module::All) {

        switch (active_task.type_) {
        case task::AllConfigUpdated: {
          VehicleConfig config;
          configGlobalRead(config);
          low_ = config.teng_low_;
          high_ = config.teng_high_;
          break;
        }

        default:
          break;
        }
      };
    }
    return 0;
  }
}
