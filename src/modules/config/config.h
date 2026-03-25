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
  ring_buffer<Task, 16> _queue;
  Task _active_task = {};
  uint8_t _task_memory[64];
  bool _task_memory_stale = true;
  int read_cfg();
  int write_cfg();
  int write_cfg(const vehicle_config &new_config);
  int handle_active_task(unsigned long timeout_ms);
  int task_config_detect_track(unsigned long timeout_ms);


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

int config::push(const Task &task) { return _queue.push(task); }

config::config() : _logger(nullptr), _valid_config(true) {}
config::config(system_logger *logger) : _logger(logger), _valid_config(true) {}

config::~config() {}

int config::read_cfg() {
  EEPROM.get(0, _config);
  return 0;
}

int config::write_cfg(const vehicle_config &new_config) {
  EEPROM.put(0, new_config);
  return 0;
}

int config::task_config_detect_track(unsigned long timeout_ms) {
  unsigned long now = millis();
  unsigned long task_timeout = now + timeout_ms;
  task_config_track_detect_data task_data;
  if (!_task_memory_stale) {
    memcpy(&task_data, _task_memory, sizeof(task_data));
  } else {
      gps_data current_gps;
      gps_global_read(current_gps);
      task_data.gps_lat = current_gps.lat.value;
      task_data.gps_lng = current_gps.lng.value;
      task_data.cos = cos(task_data.gps_lat * M_PI / 180);
  }
  while (true)
  {
    task_data.last_checked++;
    track_data temp;
    int res = this->get_track(task_data.last_checked, temp);
    if (res == 0) {
      
      double delta_lat = temp.pt_a.lat - task_data.gps_lat;
      double delta_lng = (temp.pt_a.lng - task_data.gps_lng) * task_data.cos;
      double dist2 = delta_lat*delta_lat + delta_lng*delta_lng;
      if (dist2 < task_data.sqdiff || task_data.smallest_idx == 0) {
        task_data.smallest_idx = task_data.last_checked;
        task_data.sqdiff = dist2;
      }
      if (task_data.last_checked >= 8) {
        this->load_track(task_data.smallest_idx);  
        _task_memory_stale = true;
        _active_task = {MOD_NULL, TASK_NULL, 0};
        return 0;
      }
    }
    if (millis() > task_timeout) {
      _task_memory_stale = false;
      memcpy(_task_memory, &task_data, sizeof(task_data));
      return 1;
    }
  }
}

int config::handle_active_task(unsigned long timeout_ms) {
  switch (_active_task.type)
  {
  case TASK_CONFIG_TRACK_DETECT:
    return task_config_detect_track(timeout_ms);
    break;
  
  default:
    break;
  }
  return 0;
}

int config::auto_init() {
  this->read_cfg();
  if (_config.magic != CONFIG_MAGIC) {
#ifdef WARN
    if (_logger != nullptr) {
      _logger->warn("Config invalid, overwriting");
    }
#endif
    vehicle_config clean_config;
    this->write_cfg(clean_config);
    this->read_cfg();
    if (_config.magic != CONFIG_MAGIC) {
#ifdef ERROR
      if (_logger != nullptr) {
        _logger->error("Config write failed, EEPROM may be burnt");
      }
#endif
      return 1;
    }
  }
  _valid_config = true;
  return 0;
}

int config::loop(unsigned long timeout_ms) {
  if (_active_task.type != TASK_NULL && _active_task.target != MOD_NULL) {
    this->handle_active_task(timeout_ms);
    return 0;
  }
  int res = _queue.pop(_active_task);
  if (res == 0) {

    this->handle_active_task(timeout_ms);
  }
  return 0;
}

int config::get_track(unsigned int idx, track_data &t) {
  if (idx < 1 || idx > 8) {
    return 1;
  }
  if (_config.track_slot_occupied[idx-1] == false) {
    return 1;
  }
  EEPROM.get(idx, t);
  if (t.magic != CONFIG_MAGIC) {
    return 1;
  }
  return 0;
}

int config::load_track(unsigned int idx) {
  if (idx < 1 || idx > 8) {
    return 1;
  }
  if (_config.track_slot_occupied[idx-1] == false) {
    return 1;
  }
  track_data temp;
  EEPROM.get(idx, temp);
  if (temp.magic != CONFIG_MAGIC) {
    return 1;
  }
  _loaded_track = temp;
  return 0;
}
