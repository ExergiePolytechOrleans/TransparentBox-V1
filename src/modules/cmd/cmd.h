// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <Arduino.h>
#include <avr/wdt.h>

#include "base/module_base.h"
#include "base/ring_buffer.h"
#include "base/task.h"
#include "custom_types.h"
#include "modules/logger/system_logger.h"

class Cmd : public ModuleBase {
private:
  enum CommandId {
    Unknown = 0,
    Reboot,
    PutTrack,
    DeleteTrack,
    DumpTrack,
    ConfigReset,
    ConfigDump,
    TrackAutodetect,
    DisplayGpsDebug,
    DisplayDriverPrimary,
    BatteryCal,
    BatteryPrintVbat,
    BatterySetLow,
    ThermoSetLow,
    ThermoSetHigh,
  };

  HardwareSerial *data_stream_;
  unsigned long baud_rate_ = 115200;
  SystemLogger *logger_;
  char buffer_[256];
  unsigned int index_ = 0;
  bool buffer_open_ = false;
  RingBuffer<Task, 16> queue_;

  static const unsigned short MAX_ARGS = 10;

  int tryParse();
  unsigned short splitArgs(char *input, char *argv[], unsigned short max_args);
  char *trimArg(char *input);
  CommandId parseCommandName(const char *input);
  int dispatchCommand(CommandId command, unsigned short argc, char *argv[]);
  int parseTrackSlotId(const char *id_str, unsigned short &id_out);
  int dumpTrackSlot(unsigned short id);
  int handleRebootCommand(unsigned short argc);
  int handleDumpConfigCommand(unsigned short argc);
  int handleTrackPutCommand(unsigned short argc, char *argv[]);
  int handleTrackDeleteCommand(unsigned short argc, char *argv[]);
  int handleTrackDumpCommand(unsigned short argc, char *argv[]);
  int handleConfigResetCommand(unsigned short argc);
  int handleTrackAutodetectCommand(unsigned short argc);
  int handleDisplayGpsDebug(unsigned short argc);
  int handleDisplayDriverPrimary(unsigned short argc);
  int handleBatteryCal(unsigned short argc, char *argv[]);
  int handleBatteryPrintVbat(unsigned short argc);
  int handleBatterySetLow(unsigned short argc, char *argv[]);
  int handleThermoSetLow(unsigned short argc, char *argv[]);
  int handleThermoSetHigh(unsigned short argc, char *argv[]);
  int handleUnknownCommand(unsigned short argc, char *argv[]);

public:
  int push(const Task &task) override;
  Cmd(HardwareSerial *data_stream);
  Cmd(HardwareSerial *data_stream, SystemLogger *logger);
  ~Cmd();
  int init();
  int parseTask(unsigned long timeout_ms = 500);
};
