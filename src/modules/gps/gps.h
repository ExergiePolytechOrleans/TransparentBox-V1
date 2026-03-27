// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "custom_types.h"
#include "TinyGPSPlus.h"
#include "flags.h"
#include "modules/logger/system_logger.h"
#include "base/task.h"
#include "base/ring_buffer.h"
#include "base/module_base.h"
#include "data/gps_store.h"
#include "base/router.h"

class gps : public module_base {
private:
  TinyGPSPlus *_gps;
  HardwareSerial *_data_stream;
  system_logger *_logger;
  ring_buffer<Task, 16> _queue;
  uint32_t _last_fix_val = 0;

public:
  int push(const Task &task) override;
  gps(HardwareSerial *data_stream);
  gps(HardwareSerial *data_stream, system_logger *logger);
  ~gps();
  int loop(unsigned long timeout_ms = 500);
  int init();
  gps_data get_data();
};