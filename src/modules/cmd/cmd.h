// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <Arduino.h>
#include <avr/wdt.h>

#include "custom_types.h"
#include "modules/logger/system_logger.h"
#include "base/task.h"
#include "base/ring_buffer.h"
#include "base/module_base.h"

class cmd : public module_base {
private:
  enum command_id {
    CMD_UNKNOWN = 0,
    CMD_REBOOT,
    CMD_DUMPCFG,
    CMD_PUT_TRACK,
  };

  HardwareSerial *_data_stream;
  unsigned long _baud_rate = 115200;
  system_logger *_logger;
  char _buffer[256];
  unsigned int _idx = 0;
  bool _buf_open = false;
  ring_buffer<Task, 16> _queue;

  static const unsigned short _max_args = 10;

  int try_parse();
  unsigned short split_args(char *input, char *argv[], unsigned short max_args);
  char *trim_arg(char *input);
  command_id parse_command_name(const char *input);
  int dispatch_command(command_id command, unsigned short argc, char *argv[]);

public:
  int push(const Task &task) override;
  cmd(HardwareSerial *data_stream);
  cmd(HardwareSerial *data_stream, system_logger *logger);
  ~cmd();
  int init();
  int parse_task(unsigned long timeout_ms = 500);
};