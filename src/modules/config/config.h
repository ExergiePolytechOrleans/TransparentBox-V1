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
#include "data/eeprom_layout.h"
#include "base/router.h"

struct TaskConfigTrackDetectData {
  unsigned short last_checked_ = 0;
  unsigned short smallest_idx_ = 0;
  double cos_ = 0;
  double sqdiff_ = 0;
  double gps_lat_ = 0;
  double gps_lng_ = 0;
};

class Config : public ModuleBase {
private:
  VehicleConfig config_;
  SystemLogger *logger_;
  bool valid_config_;
  TrackData loaded_track_;
  bool is_track_loaded_ = false;
  RingBuffer<Task, 16> queue_;
  Task active_task_ = {};
  uint8_t task_memory_[64] = {};
  bool task_memory_stale_ = true;
  bool no_tracks_notice_shown_ = false;

  int readConfig();
  int writeConfig();
  int writeConfig(const VehicleConfig &new_config);
  int handleActiveTask(unsigned long timeout_ms);
  int taskConfigDetectTrack(unsigned long timeout_ms);
  int taskComplete();
  int writeTrack(const TrackData& track_data);
  int writeTrackFromTemp();
  int deleteTrack(unsigned short idx);
  int resetConfig();
  int writeVbatCal(double value);
  int writeVbatLow(double value);
  int writeTengLow(double value);
  int writeTengHigh(double value);

public:
  int push(const Task &task) override;
  Config();
  Config(SystemLogger *logger);
  ~Config();
  int autoInit();
  int loop(unsigned long timeout_ms = 500);
  int getTrack(unsigned int idx, TrackData &track_data);
  int loadTrack(unsigned int idx);
};
