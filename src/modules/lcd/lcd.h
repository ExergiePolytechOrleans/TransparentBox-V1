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

namespace screen {

typedef enum lcd_screen {
  blank,
  gps_debug,
  msg_gps_fix,
  msg_track_detect_ok,
} lcd_screen;

} // namespace screen

class lcd : public module_base {
private:
  LiquidCrystal_I2C *_display;
  bool _dispaly_cleared;
  system_logger *_logger = nullptr;
  screen::lcd_screen _screen;
  screen::lcd_screen _previous_screen;
  unsigned long _last_render;
  unsigned long _frame_duration;
  long _hold_till_frame = -1;
  ring_buffer<Task, 16> _queue;
  uint32_t _frame_ctr = 0;

  void clear();
  void print(const String &msg);
  void print(char c);
  void print(const char c[]);
  void print(double d, int digits = 2);
  void print(unsigned long l, int base = 10);
  void print(long l, int base = 10);
  void print(unsigned int i, int base = 10);
  void print(int i, int base = 10);

  int render_gps_debug();
  int render_msg_gps_fix();
  int render_msg_track_detect_ok();

public:
  int push(const Task &task) override;
  lcd();
  lcd(system_logger *logger);
  ~lcd();
  int init();
  int print_message(String message);
  int loop(unsigned long timeout_ms = 500);
};