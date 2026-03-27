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

class battery : public module_base {

private:
  system_logger *_logger;
  ring_buffer<Task, 16> _queue;
  double _vbat = 0;
  double _cal = 0;
  unsigned long _update_interval = 1000;
  unsigned long _last_read = 0;
  int calibrate(const Task& task);

public:
  int push(const Task &task) override;
  battery();
  battery(system_logger *logger);
  ~battery();
  int init();
  int loop(unsigned long timeout_ms = 500);
};