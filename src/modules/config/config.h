// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "base/module_base.h"
#include "base/ring_buffer.h"
#include "base/task.h"
#include "custom_types.h"
#include "flags.h"
#include "modules/logger/system_logger.h"
#include <EEPROM.h>
#include "data/gps_store.h"
#include "data/track_store.h"
#include "base/router.h"

struct task_config_track_detect_data {
  unsigned short last_checked = 0;
  unsigned short smallest_idx = 0;
  double cos;
  double sqdiff = 0;
  double gps_lat;
  double gps_lng;
};

class config : public module_base {
private:
  vehicle_config _config;
  system_logger *_logger;
  bool _valid_config;
  track_data _loaded_track;
  bool _is_track_loaded = false;
  ring_buffer<Task, 16> _queue;
  Task _active_task = {};
  uint8_t _task_memory[64] = {};
  bool _task_memory_stale = true;
  bool _no_tracks_notice_shown = false;

  int read_cfg();
  int write_cfg();
  int write_cfg(const vehicle_config &new_config);
  int handle_active_task(unsigned long timeout_ms);
  int task_config_detect_track(unsigned long timeout_ms);
  int task_complete();
  int write_track(const track_data& in);
  int write_track_from_temp();

public:
  int push(const Task &task) override;
  config();
  config(system_logger *logger);
  ~config();
  int auto_init();
  int loop(unsigned long timeout_ms = 500);
  int get_track(unsigned int idx, track_data &t);
  int load_track(unsigned int idx);
};
