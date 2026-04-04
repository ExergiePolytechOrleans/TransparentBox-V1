// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "base/module_base.h"
#include "base/ring_buffer.h"
#include "modules/logger/system_logger.h"

#define INJ_GPIO 37

class InjectionCounter : public ModuleBase {
private:
  SystemLogger *logger_;
  RingBuffer<Task, 16> queue_;
  unsigned long last_check_;
  unsigned long check_interval_ = 100;
  unsigned long debounce_ = 100;
  int last_switch_ = 0;
  int last_switch_time_ = 0;
  bool waiting_debounce_ = 0;
  uint16_t counter_ = 0;

public:
  int push(const Task &task) override;
  InjectionCounter();
  InjectionCounter(SystemLogger* logger);
  ~InjectionCounter();
  int init();
  int loop();
};