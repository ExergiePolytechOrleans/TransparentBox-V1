// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "flags.h"
#include "modules/logger/system_logger.h"
#include "modules/gps/gps.h"
#include "base/task.h"
#include "base/ring_buffer.h"
#include "base/module_base.h"
#include "data/gps_store.h"
#include "data/track_store.h"
#include "data/general_store.h"

namespace screen {

enum LcdScreen : uint8_t {
  Blank,
  GpsDebug,
  DriverPrimary,
  MsgGpsFix,
  MsgGpsTrigger,
  MsgTrackDetectOk,
  MsgConfigNoTracks,
  MsgBatteryLow,
  MsgEngineTempLow,
  MsgEngineTempHigh,
  MsgLapCounterStart,
  MsgLapCounterLapTime,
};

} // namespace screen

class Lcd : public ModuleBase {
private:
  LiquidCrystal_I2C *display_;
  bool display_cleared_;
  SystemLogger *logger_ = nullptr;
  screen::LcdScreen screen_;
  screen::LcdScreen data_screen_;
  screen::LcdScreen message_screen_;
  unsigned long last_render_;
  unsigned long frame_duration_;
  unsigned long message_end_ = 0;
  bool message_active_ = false;
  bool force_render_ = false;
  RingBuffer<Task, 16> queue_;
  Task deferred_task_{};
  bool deferred_task_valid_ = false;
  bool base_rendered_ = false;

  void clear();
  void print(const String &msg);
  void print(char c);
  void print(const char c[]);
  void print(float d, int digits = 2);
  void print(unsigned long l, int base = 10);
  void print(long l, int base = 10);
  void print(unsigned int i, int base = 10);
  void print(int i, int base = 10);

  bool isMessageTask(task::Type type);
  void activateMessage(screen::LcdScreen msg_screen, unsigned long duration_ms);
  void expireMessageIfNeeded(unsigned long now);
  screen::LcdScreen getActiveScreen() const;

  int renderGpsDebug();
  int renderDriverPrimary();
  int renderMsgGpsFix();
  int renderMsgGpsTrigger();
  int renderMsgTrackDetectOk();
  int renderMsgConfigNoTracks();
  int renderMsgBatteryLow();
  int renderMsgEngineTempLow();
  int renderMsgEngineTempHigh();
  int renderMsgLapCounterStart();
  int renderMsgLapCounterLapTime();

public:
  int push(const Task &task) override;
  Lcd();
  Lcd(SystemLogger *logger);
  ~Lcd();
  int init();
  int printMessage(String message);
  int loop(unsigned long timeout_ms = 500);
};
