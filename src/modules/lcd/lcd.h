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

typedef enum lcd_screen {
  blank,
  gps_debug,
  driver_primary,
  msg_gps_fix,
  msg_track_detect_ok,
  msg_config_no_tracks,
} lcd_screen;

} // namespace screen

class lcd : public module_base {
private:
  LiquidCrystal_I2C *_display;
  bool _dispaly_cleared;
  system_logger *_logger = nullptr;
  screen::lcd_screen _screen;
  screen::lcd_screen _data_screen;
  screen::lcd_screen _msg_screen;
  unsigned long _last_render;
  unsigned long _frame_duration;
  unsigned long _msg_end = 0;
  bool _msg_active = false;
  bool _force_render = false;
  ring_buffer<Task, 16> _queue;
  Task _deferred_task{};
  bool _deferred_task_valid = false;

  void clear();
  void print(const String &msg);
  void print(char c);
  void print(const char c[]);
  void print(double d, int digits = 2);
  void print(unsigned long l, int base = 10);
  void print(long l, int base = 10);
  void print(unsigned int i, int base = 10);
  void print(int i, int base = 10);

  bool is_message_task(task_type type);
  void activate_message(screen::lcd_screen msg_screen, unsigned long duration_ms);
  void expire_message_if_needed(unsigned long now);
  screen::lcd_screen get_active_screen() const;

  int render_gps_debug();
  int render_driver_primary();
  int render_msg_gps_fix();
  int render_msg_track_detect_ok();
  int render_msg_config_no_tracks();

public:
  int push(const Task &task) override;
  lcd();
  lcd(system_logger *logger);
  ~lcd();
  int init();
  int print_message(String message);
  int loop(unsigned long timeout_ms = 500);
};
