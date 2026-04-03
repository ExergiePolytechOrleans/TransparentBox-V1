// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "custom_types.h"
#include "TinyGPSPlus.h"
#include "flags.h"
#include "modules/logger/system_logger.h"
#include "base/task.h"
#include "base/ring_buffer.h"
#include "base/module_base.h"
#include "data/gps_store.h"
#include "data/general_store.h"
#include "base/router.h"

namespace trigger_status {
  
enum TriggerStatus {
  Idle = 0,
  Armed = 1,
  Trigd = 2,
};
}

class Gps : public ModuleBase {
private:
  TinyGPSPlus *gps_;
  HardwareSerial *data_stream_;
  SystemLogger *logger_;
  RingBuffer<Task, 16> queue_;
  uint32_t last_fix_value_ = 0;
  trigger_status::TriggerStatus start_line_trigger_ = trigger_status::Idle;

  LatLng track_point_a_;
  LatLng track_point_b_;
  Vec2 track_vec_b_;
  Vec2 track_vec_center_;
  float start_line_length_ = 0;
  float start_line_sq_dist_ = 0;

  bool lap_active_ = false;
  unsigned long last_check_ = 0;
  unsigned long check_interval_ = 100;
  unsigned long state_changed_at_ = 0;
  int arm_sign_ = 0;


public:
  int push(const Task &task) override;
  Gps(HardwareSerial *data_stream);
  Gps(HardwareSerial *data_stream, SystemLogger *logger);
  ~Gps();
  int loop(unsigned long timeout_ms = 500);
  int init();
  GpsData getData();
};
