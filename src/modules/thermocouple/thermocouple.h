// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#define THERMO_CS A1
#define THERMO_SO A0
#define THERMO_SCK A2
#include "base/module_base.h"
#include "base/ring_buffer.h"
#include "custom_types.h"
#include "modules/logger/system_logger.h"
#include <Arduino.h>
#include <max6675.h>

class thermocouple : public module_base {
private:
  system_logger *_logger;
  ring_buffer<Task, 16> _queue;
  MAX6675 *_thermocouple;
  double _temp;
  unsigned long _update_interval = 1000;
  unsigned long _last_read = 0;

public:
  int push(const Task &task) override;
  thermocouple();
  thermocouple(system_logger *logger);
  ~thermocouple();
  int init();
  int loop(unsigned long timeout_ms = 500);
};