// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "base/module_base.h"
#include "modules/logger/system_logger.h"
#include <Arduino.h>

class battery : public module_base {

private:
  system_logger *_logger;
  ring_buffer<Task, 16> _queue;

public:
  int push(const Task &task) override;
  battery();
  battery(system_logger *logger);
  ~battery();
};