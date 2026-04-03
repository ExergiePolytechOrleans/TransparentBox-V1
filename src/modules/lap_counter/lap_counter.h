// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "base/module_base.h"
#include "base/ring_buffer.h"
#include "custom_types.h"
#include "modules/logger/system_logger.h"

class LapCounter : public ModuleBase {
private:
  SystemLogger *logger_;
  RingBuffer<Task, 16> queue_;

public:
  int push(const Task &task) override;
  LapCounter();
  LapCounter(SystemLogger *logger);
  ~LapCounter();
  int init();
  int loop();
};