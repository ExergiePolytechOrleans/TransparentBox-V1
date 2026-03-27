// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "config.h"
#include "data/track_store.h"

#include <math.h>
#include <string.h>

int config::push(const Task &task) { return _queue.push(task); }

int config::task_complete() {
  _task_memory_stale = true;
  _active_task = {MOD_NULL, TASK_NULL, 0};
  return 0;
}

int config::write_track(const track_data &in) {
  track_data copy = in;
  copy.magic = CONFIG_MAGIC;
  if (copy.id < 1 || copy.id > 8) {
#ifdef ERROR
    if (_logger != nullptr) {
      _logger->error("Cannot write track data with out of range id, aborting");
    }
#endif
    return 1;
  }
  EEPROM.put(eeprom_layout::track_slot_addr(copy.id), copy);
  _config.track_slot_occupied[copy.id - 1] = true;
  this->write_cfg();
#ifdef INFO
  if (_logger != nullptr) {
    _logger->info("Succesfully wrote new track into slot " + String(copy.id));
  }
#endif
  return 0;
}

int config::write_track_from_temp() {
  track_data new_track;
  track_temp_global_read(new_track);
  return this->write_track(new_track);
}

int config::delete_track(unsigned short idx) {
  if (idx < 1 || idx > 8) {
#ifdef ERROR
    if (_logger != nullptr) {
      _logger->error("Cannot delete track with out of range id, aborting");
    }
#endif
    return 1;
  }

  if (_config.track_slot_occupied[idx - 1] == false) {
#ifdef WARN
    if (_logger != nullptr) {
      _logger->warn("Requested delete on empty track slot " + String(idx));
    }
#endif
    return 0;
  }

  _config.track_slot_occupied[idx - 1] = false;

  if (_is_track_loaded && _loaded_track.id == idx) {
    _is_track_loaded = false;
    _loaded_track = {};
    track_global_write(_loaded_track);
  }

  int write_res = this->write_cfg();

#ifdef INFO
  if (_logger != nullptr && write_res == 0) {
    _logger->info("Succesfully deleted track slot " + String(idx));
  }
#endif
  return write_res;
}

int config::reset_cfg() {
  vehicle_config clean_config;
  _config = clean_config;
  _is_track_loaded = false;
  _loaded_track = {};
  _task_memory_stale = true;
  _no_tracks_notice_shown = false;
  track_global_write(_loaded_track);

#ifdef INFO
  if (_logger != nullptr) {
    _logger->info("Resetting configuration to factory defaults");
  }
#endif

  return this->write_cfg();
}

int config::write_vbat_cal(double val) {
  _config.vbat_calibration = val;
  return this->write_cfg();
}

int config::write_vbat_low(double val) {
  _config.vbat_low = val;
  return this->write_cfg();
}

int config::write_teng_low(double val) {
  _config.teng_low = val;
  return this->write_cfg();
}

int config::write_teng_high(double val) {
  _config.teng_high = val;
  return this->write_cfg();
}

config::config() : _logger(nullptr), _valid_config(true) {}

config::config(system_logger *logger) : _logger(logger), _valid_config(true) {}

config::~config() {}

int config::read_cfg() {
  EEPROM.get(eeprom_layout::config_addr, _config);
  config_global_write(_config);
  return 0;
}

int config::write_cfg() {
  EEPROM.put(eeprom_layout::config_addr, _config);
  config_global_write(_config);
#ifdef INFO
  if (_logger != nullptr) {
    _logger->info("Config updated and saved to EEPROM");
  }
#endif
  router::send(MOD_ALL, TASK_ALL_CONFIG_UPDATED);
  return 0;
}

int config::write_cfg(const vehicle_config &new_config) {
  _config = new_config;
  EEPROM.put(eeprom_layout::config_addr, new_config);
  config_global_write(new_config);
  return 0;
}

int config::task_config_detect_track(unsigned long timeout_ms) {
  unsigned long start = millis();
  bool min_one = false;

  for (size_t i = 0; i < 8; i++) {
    if (_config.track_slot_occupied[i] == true) {
      min_one = true;
      break;
    }
  }

  if (!min_one) {
    if (!_no_tracks_notice_shown) {
      router::send(MOD_LCD, TASK_DISPLAY_MSG_CONFIG_NO_TRACKS, 3000);
      _no_tracks_notice_shown = true;
    }
    this->task_complete();
    return 1;
  }

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

  while (true) {
    task_data.last_checked++;

    track_data temp;
    int res = this->get_track(task_data.last_checked, temp);
    if (res == 0) {
      double delta_lat = temp.pt_a.lat - task_data.gps_lat;
      double delta_lng = (temp.pt_a.lng - task_data.gps_lng) * task_data.cos;
      double dist2 = delta_lat * delta_lat + delta_lng * delta_lng;

      if (dist2 < task_data.sqdiff || task_data.smallest_idx == 0) {
        task_data.smallest_idx = task_data.last_checked;
        task_data.sqdiff = dist2;
      }
    }

    if (task_data.last_checked >= 8) {
      int load_res = 1;
      if (task_data.smallest_idx > 0) {
        load_res = this->load_track(task_data.smallest_idx);
      }

      this->task_complete();
      if (load_res == 0) {
        _no_tracks_notice_shown = false;
        router::send(MOD_LCD, TASK_DISPLAY_MSG_TRACK_DETECT_OK, 4000);
        return 0;
      }

      if (!_no_tracks_notice_shown) {
        router::send(MOD_LCD, TASK_DISPLAY_MSG_CONFIG_NO_TRACKS, 3000);
        _no_tracks_notice_shown = true;
      }
      return 1;
    }

    if ((unsigned long)(millis() - start) >= timeout_ms) {
      _task_memory_stale = false;
      memcpy(_task_memory, &task_data, sizeof(task_data));
      return 1;
    }
  }
}

int config::handle_active_task(unsigned long timeout_ms) {
  switch (_active_task.type) {
  case TASK_CONFIG_TRACK_DETECT: {
    if (!_is_track_loaded || _active_task.data == 1) {
      return task_config_detect_track(timeout_ms);
    }
    this->task_complete();
    return 0;
  }

  case TASK_CONFIG_WRITE_TEMP_TRACK: {
    int res = this->write_track_from_temp();
    this->task_complete();
    return res;
  }

  case TASK_CONFIG_TRACK_DELETE: {
    int res = this->delete_track(_active_task.data);
    this->task_complete();
    return res;
  }

  case TASK_CONFIG_CFG_RESET: {
    int res = this->reset_cfg();
    this->task_complete();
    return res;
  }

  case TASK_CONFIG_VBAT_CAL_SET: {
    double cal_value;
    memcpy(&cal_value, &_active_task.data, sizeof(double));
    int res = this->write_vbat_cal(cal_value);
    this->task_complete();
    return res;
  }

  case TASK_CONFIG_VBAT_SET_LOW: {
    double low_value;
    memcpy(&low_value, &_active_task.data, sizeof(double));
    int res = this->write_vbat_low(low_value);
    this->task_complete();
    return res;
  }

  case TASK_CONFIG_TENG_SET_LOW: {
    double low_value;
    memcpy(&low_value, &_active_task.data, sizeof(double));
    int res = this->write_teng_low(low_value);
    this->task_complete();
    return res;
  }

  case TASK_CONFIG_TENG_SET_HIGH: {
    double high_value;
    memcpy(&high_value, &_active_task.data, sizeof(double));
    int res = this->write_teng_high(high_value);
    this->task_complete();
    return res;
  }

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

  if (_config.track_slot_occupied[idx - 1] == false) {
    return 1;
  }

  EEPROM.get(eeprom_layout::track_slot_addr(idx), t);
  if (t.magic != CONFIG_MAGIC) {
    return 1;
  }

  return 0;
}

int config::load_track(unsigned int idx) {
  if (idx < 1 || idx > 8) {
    return 1;
  }

  if (_config.track_slot_occupied[idx - 1] == false) {
    return 1;
  }

  track_data temp;
  EEPROM.get(eeprom_layout::track_slot_addr(idx), temp);
  if (temp.magic != CONFIG_MAGIC) {
    return 1;
  }

  _loaded_track = temp;
  track_global_write(_loaded_track);
  _is_track_loaded = true;
  return 0;
}
