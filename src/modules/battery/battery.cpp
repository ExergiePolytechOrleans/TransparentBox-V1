// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "battery.h"
#include "base/router.h"
#include "data/config_store.h"
#include "data/general_store.h"

int Battery::calibrate(const Task &task) {
  float actual_voltage;
  memcpy(&actual_voltage, &task.data_, sizeof(float));
  int adc_read = analogRead(VBAT_PIN);
  float cal_factor = actual_voltage / adc_read;
  uint32_t output_val;
  memcpy(&output_val, &cal_factor, sizeof(uint32_t));
  router::send(module::Config, task::ConfigVbatCalSet, output_val);
  return 0;
}

int Battery::push(const Task &task) { return queue_.push(task); }

Battery::Battery() : logger_(nullptr) {}

Battery::Battery(SystemLogger *logger) : logger_(logger) {}

Battery::~Battery() {}

int Battery::init() {
  pinMode(VBAT_PIN, INPUT);
  VehicleConfig config;
  configGlobalRead(config);
  calibration_ = config.vbat_calibration_;
  low_threshold_ = config.vbat_low_;
  return 0;
}

int Battery::loop(unsigned long timeout_ms) {
  (void)timeout_ms;

  if (millis() > last_read_at_ + update_interval_) {
    int adc_read = analogRead(VBAT_PIN);
    vbat_ = calibration_ * adc_read;

    vbatGlobalWrite(vbat_);
    if (vbat_ < low_threshold_) {
      if (warning_sent_at_ == 0 ||
          millis() > warning_sent_at_ + warning_timeout_) {
        router::send(module::Lcd, task::DisplayMsgBatteryLow, 2000);
        warning_sent_at_ = millis();
      }
    }
    last_read_at_ = millis();
  }

  Task active_task;
  int res = queue_.pop(active_task);
  if (res == 0) {
    if (active_task.target_ == module::Battery) {

      switch (active_task.type_) {
      case task::BatteryCal:
        this->calibrate(active_task);
        break;

      default:
        break;
      }
    } else if (active_task.target_ == module::All) {
      switch (active_task.type_) {
      case task::AllConfigUpdated: {
        VehicleConfig config;
        configGlobalRead(config);
        calibration_ = config.vbat_calibration_;
        low_threshold_ = config.vbat_low_;
        break;
      }

      default:
        break;
      }
    }
  }
  return 0;
}
