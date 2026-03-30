// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "base/module_base.h"
#include "base/ring_buffer.h"
#include "modules/logger/system_logger.h"
#include "telemetry_common/telemetry_common.h"
#include <Arduino.h>
#include "custom_types.h"
#include "data/gps_store.h"

class Telemetry : public ModuleBase {
private:
  SystemLogger *logger_;
  RingBuffer<Task, 16> queue_;
  HardwareSerial *data_stream_;
  unsigned long last_sent_ = 0;
  unsigned long update_interval_ = 1000;
  TelemetryLoRaHeader lora_header_;
  TelemetryUARTHeader uart_header_;

public:
  int push(const Task &task) override;
  Telemetry(HardwareSerial *data_stream);
  Telemetry(HardwareSerial *data_stream, SystemLogger *logger);
  ~Telemetry();
  int init();
  int loop();
};