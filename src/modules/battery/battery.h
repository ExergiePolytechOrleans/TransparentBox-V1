// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#define VBAT_PIN A3
#include "base/module_base.h"
#include "base/ring_buffer.h"
#include "custom_types.h"
#include "modules/logger/system_logger.h"
#include <Arduino.h>

class Battery : public ModuleBase {

private:
  SystemLogger *logger_;
  RingBuffer<Task, 16> queue_;
  double vbat_ = 0;
  double calibration_ = 0;
  double low_threshold_ = 0;
  unsigned long warning_sent_at_ = 0;
  unsigned long warning_timeout_ = 10000;
  unsigned long update_interval_ = 1000;
  unsigned long last_read_at_ = 0;
  int calibrate(const Task& task);

public:
  int push(const Task &task) override;
  Battery();
  Battery(SystemLogger *logger);
  ~Battery();
  int init();
  int loop(unsigned long timeout_ms = 500);
};
