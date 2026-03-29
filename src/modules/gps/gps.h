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

class Gps : public ModuleBase {
private:
  TinyGPSPlus *gps_;
  HardwareSerial *data_stream_;
  SystemLogger *logger_;
  RingBuffer<Task, 16> queue_;
  uint32_t last_fix_value_ = 0;

public:
  int push(const Task &task) override;
  Gps(HardwareSerial *data_stream);
  Gps(HardwareSerial *data_stream, SystemLogger *logger);
  ~Gps();
  int loop(unsigned long timeout_ms = 500);
  int init();
  GpsData getData();
};
