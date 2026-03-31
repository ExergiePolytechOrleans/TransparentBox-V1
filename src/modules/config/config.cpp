// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "config.h"
#include "data/track_store.h"

#include <math.h>
#include <string.h>

int Config::push(const Task &task) { return queue_.push(task); }

int Config::taskComplete() {
  task_memory_stale_ = true;
  active_task_ = Task(module::Null, task::Null, 0);
  return 0;
}

int Config::writeTrack(const TrackData &track_data) {
  TrackData track_copy = track_data;
  track_copy.magic_ = CONFIG_MAGIC;
  if (track_copy.id_ < 1 || track_copy.id_ > 8) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("Cannot write track data with out of range id, aborting");
    }
#endif
    return 1;
  }
  EEPROM.put(eeprom_layout::trackSlotAddr(track_copy.id_), track_copy);
  config_.track_slot_occupied_[track_copy.id_ - 1] = true;
  this->writeConfig();
#ifdef INFO
  if (logger_ != nullptr) {
    logger_->info("Succesfully wrote new track into slot " + String(track_copy.id_));
  }
#endif
  return 0;
}

int Config::writeTrackFromTemp() {
  TrackData track_data;
  trackTempGlobalRead(track_data);
  return this->writeTrack(track_data);
}

int Config::deleteTrack(unsigned short idx) {
  if (idx < 1 || idx > 8) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("Cannot delete track with out of range id, aborting");
    }
#endif
    return 1;
  }

  if (config_.track_slot_occupied_[idx - 1] == false) {
#ifdef WARN
    if (logger_ != nullptr) {
      logger_->warn("Requested delete on empty track slot " + String(idx));
    }
#endif
    return 0;
  }

  config_.track_slot_occupied_[idx - 1] = false;

  if (is_track_loaded_ && loaded_track_.id_ == idx) {
    is_track_loaded_ = false;
    loaded_track_ = {};
    GlobalTrackData track;
    track.loaded_ = false;
    track.root_ = loaded_track_;
    trackGlobalWrite(track);
  }

  int write_result = this->writeConfig();

#ifdef INFO
  if (logger_ != nullptr && write_result == 0) {
    logger_->info("Succesfully deleted track slot " + String(idx));
  }
#endif
  return write_result;
}

int Config::resetConfig() {
  VehicleConfig clean_config;
  config_ = clean_config;
  is_track_loaded_ = false;
  loaded_track_ = {};
  task_memory_stale_ = true;
  no_tracks_notice_shown_ = false;
    GlobalTrackData track;
    track.loaded_ = false;
    track.root_ = loaded_track_;
    trackGlobalWrite(track);

#ifdef INFO
  if (logger_ != nullptr) {
    logger_->info("Resetting configuration to factory defaults");
  }
#endif

  return this->writeConfig();
}

int Config::writeVbatCal(float value) {
  config_.vbat_calibration_ = value;
  return this->writeConfig();
}

int Config::writeVbatLow(float value) {
  config_.vbat_low_ = value;
  return this->writeConfig();
}

int Config::writeTengLow(float value) {
  config_.teng_low_ = value;
  return this->writeConfig();
}

int Config::writeTengHigh(float value) {
  config_.teng_high_ = value;
  return this->writeConfig();
}

Config::Config() : logger_(nullptr), valid_config_(true) {}

Config::Config(SystemLogger *logger) : logger_(logger), valid_config_(true) {}

Config::~Config() {}

int Config::readConfig() {
  EEPROM.get(eeprom_layout::CONFIG_ADDR, config_);
  configGlobalWrite(config_);
  return 0;
}

int Config::writeConfig() {
  EEPROM.put(eeprom_layout::CONFIG_ADDR, config_);
  configGlobalWrite(config_);
#ifdef INFO
  if (logger_ != nullptr) {
    logger_->info("Config updated and saved to EEPROM");
  }
#endif
  router::send(module::All, task::AllConfigUpdated);
  return 0;
}

int Config::writeConfig(const VehicleConfig &new_config) {
  config_ = new_config;
  EEPROM.put(eeprom_layout::CONFIG_ADDR, new_config);
  configGlobalWrite(new_config);
  return 0;
}

int Config::taskConfigDetectTrack(unsigned long timeout_ms) {
  unsigned long start = millis();
  bool has_at_least_one_track = false;

  for (size_t i = 0; i < 8; i++) {
    if (config_.track_slot_occupied_[i] == true) {
      has_at_least_one_track = true;
      break;
    }
  }

  if (!has_at_least_one_track) {
    if (!no_tracks_notice_shown_) {
      router::send(module::Lcd, task::DisplayMsgConfigNoTracks, 3000);
      no_tracks_notice_shown_ = true;
    }
    this->taskComplete();
    return 1;
  }

  TaskConfigTrackDetectData task_data;

  if (!task_memory_stale_) {
    memcpy(&task_data, task_memory_, sizeof(task_data));
  } else {
    GpsData current_gps;
    gpsGlobalRead(current_gps);
    task_data.gps_lat_ = current_gps.lat_.value_;
    task_data.gps_lng_ = current_gps.lng_.value_;
    task_data.cos_ = cos(task_data.gps_lat_ * M_PI / 180);
  }

  while (true) {
    task_data.last_checked_++;

    TrackData track_data;
    int result = this->getTrack(task_data.last_checked_, track_data);
    if (result == 0) {
      float delta_lat = track_data.point_a_.lat_ - task_data.gps_lat_;
      float delta_lng = (track_data.point_a_.lng_ - task_data.gps_lng_) * task_data.cos_;
      float dist2 = delta_lat * delta_lat + delta_lng * delta_lng;

      if (dist2 < task_data.sqdiff_ || task_data.smallest_idx_ == 0) {
        task_data.smallest_idx_ = task_data.last_checked_;
        task_data.sqdiff_ = dist2;
      }
    }

    if (task_data.last_checked_ >= 8) {
      int load_result = 1;
      if (task_data.smallest_idx_ > 0) {
        load_result = this->loadTrack(task_data.smallest_idx_);
      }

      this->taskComplete();
      if (load_result == 0) {
        no_tracks_notice_shown_ = false;
        #ifdef INFO
        if (logger_ != nullptr) {
          logger_->info("Track detected: " + String(loaded_track_.name_));
        }
        #endif
        router::send(module::Lcd, task::DisplayMsgTrackDetectOk, 4000);
        return 0;
      }

      if (!no_tracks_notice_shown_) {
        router::send(module::Lcd, task::DisplayMsgConfigNoTracks, 3000);
        no_tracks_notice_shown_ = true;
      }
      return 1;
    }

    if ((unsigned long)(millis() - start) >= timeout_ms) {
      task_memory_stale_ = false;
      memcpy(task_memory_, &task_data, sizeof(task_data));
      return 1;
    }
  }
}

int Config::handleActiveTask(unsigned long timeout_ms) {
  switch (active_task_.type_) {
  case task::ConfigTrackDetect: {
    if (!is_track_loaded_ || active_task_.data_ == 1) {
      #ifdef DEBUG
      if (logger_ != nullptr) {
        logger_->debug("Detect track called");
      }
      #endif
      return taskConfigDetectTrack(timeout_ms);
    }
    this->taskComplete();
    return 0;
  }

  case task::ConfigWriteTempTrack: {
    int result = this->writeTrackFromTemp();
    this->taskComplete();
    return result;
  }

  case task::ConfigTrackDelete: {
    int result = this->deleteTrack(active_task_.data_);
    this->taskComplete();
    return result;
  }

  case task::ConfigReset: {
    int result = this->resetConfig();
    this->taskComplete();
    return result;
  }

  case task::ConfigVbatCalSet: {
    float cal_value;
    memcpy(&cal_value, &active_task_.data_, sizeof(float));
    int result = this->writeVbatCal(cal_value);
    this->taskComplete();
    return result;
  }

  case task::ConfigVbatSetLow: {
    float low_value;
    memcpy(&low_value, &active_task_.data_, sizeof(float));
    int result = this->writeVbatLow(low_value);
    this->taskComplete();
    return result;
  }

  case task::ConfigTengSetLow: {
    float low_value;
    memcpy(&low_value, &active_task_.data_, sizeof(float));
    int result = this->writeTengLow(low_value);
    this->taskComplete();
    return result;
  }

  case task::ConfigTengSetHigh: {
    float high_value;
    memcpy(&high_value, &active_task_.data_, sizeof(float));
    int result = this->writeTengHigh(high_value);
    this->taskComplete();
    return result;
  }

  default:
    break;
  }
  return 0;
}

int Config::autoInit() {
  this->readConfig();
  if (config_.magic_ != CONFIG_MAGIC) {
#ifdef WARN
    if (logger_ != nullptr) {
      logger_->warn("Config invalid, overwriting");
    }
#endif
    VehicleConfig clean_config;
    this->writeConfig(clean_config);
    this->readConfig();
    if (config_.magic_ != CONFIG_MAGIC) {
#ifdef ERROR
      if (logger_ != nullptr) {
        logger_->error("Config write failed, EEPROM may be burnt");
      }
#endif
      return 1;
    }
  }
  valid_config_ = true;
  return 0;
}

int Config::loop(unsigned long timeout_ms) {
  if (active_task_.type_ != task::Null && active_task_.target_ != module::Null) {
    this->handleActiveTask(timeout_ms);
    return 0;
  }

  int result = queue_.pop(active_task_);
  if (result == 0) {
    this->handleActiveTask(timeout_ms);
  }

  return 0;
}

int Config::getTrack(unsigned int idx, TrackData &track_data) {
  if (idx < 1 || idx > 8) {
    return 1;
  }

  if (config_.track_slot_occupied_[idx - 1] == false) {
    return 1;
  }

  EEPROM.get(eeprom_layout::trackSlotAddr(idx), track_data);
  if (track_data.magic_ != CONFIG_MAGIC) {
    return 1;
  }

  return 0;
}

int Config::loadTrack(unsigned int idx) {
  if (idx < 1 || idx > 8) {
    return 1;
  }

  if (config_.track_slot_occupied_[idx - 1] == false) {
    return 1;
  }

  TrackData track_data;
  EEPROM.get(eeprom_layout::trackSlotAddr(idx), track_data);
  if (track_data.magic_ != CONFIG_MAGIC) {
    return 1;
  }

  loaded_track_ = track_data;
  GlobalTrackData track;

  track.loaded_ = true;

  Vec2 point_b =eqRectProjection(track_data.point_b_, track_data.point_a_);
  track.center_ = vec2Midpoint(point_b, (Vec2){0.0f,0.0f});
  float radius = max(10.0f, vecMod(point_b) * 1.25f);
  track.circle_radius_sq_ = radius*radius;

  track.root_ = track_data;
  trackGlobalWrite(track);
  is_track_loaded_ = true;
  router::send(module::All, task::AllTrackLoaded);
  return 0;
}
