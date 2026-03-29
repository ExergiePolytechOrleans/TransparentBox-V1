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

class Thermocouple : public ModuleBase {
private:
  SystemLogger *logger_;
  RingBuffer<Task, 16> queue_;
  MAX6675 *thermocouple_;
  double temperature_;
  unsigned long update_interval_ = 1000;
  unsigned long last_read_at_ = 0;

public:
  int push(const Task &task) override;
  Thermocouple();
  Thermocouple(SystemLogger *logger);
  ~Thermocouple();
  int init();
  int loop(unsigned long timeout_ms = 500);
};
