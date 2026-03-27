// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "battery.h"
#include "base/router.h"
#include "data/config_store.h"
#include "data/general_store.h"

int battery::calibrate(const Task &task) {
  double actual_voltage;
  memcpy(&actual_voltage, &task.data, sizeof(double));
  int adc_read = analogRead(VBAT_PIN);
  double cal_factor = actual_voltage / adc_read;
  uint32_t output_val;
  memcpy(&output_val, &cal_factor, sizeof(uint32_t));
  router::send(MOD_CFG, TASK_CONFIG_VBAT_CAL_SET, output_val);
  return 0;
}

int battery::push(const Task &task) { return _queue.push(task); }

battery::battery() : _logger(nullptr) {}

battery::battery(system_logger *logger) : _logger(logger) {}

battery::~battery() {}

int battery::init() {
  pinMode(VBAT_PIN, INPUT);
  vehicle_config config;
  config_global_read(config);
  _cal = config.vbat_calibration;
}

int battery::loop(unsigned long timeout_ms) {
  if (millis() > _last_read + _update_interval) {
    int adc_read = analogRead(VBAT_PIN);
    _vbat = _cal * adc_read;

    vbat_global_write(_vbat);
  }
  Task active_task;
  int res = _queue.pop(active_task);
  if (res == 0) {
    if (active_task.target == MOD_BAT) {

      switch (active_task.type) {
      case TASK_BATTERY_CAL:
        this->calibrate(active_task);
        break;

      default:
        break;
      }
    } else if (active_task.target == MOD_ALL) {
        switch (active_task.type)
        {
        case TASK_ALL_CONFIG_UPDATED: {
            vehicle_config config; 
            config_global_read(config);
            _cal = config.vbat_calibration;
            break;
        }
        
        default:
            break;
        }
    }
  }
  return 0;
}