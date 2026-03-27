// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "gps.h"

#define MOD "modules/gps/gps.h"

int gps::push(const Task &task) {
  return _queue.push(task);
}

gps::gps(HardwareSerial *data_stream)
    : _gps(nullptr), _data_stream(data_stream), _logger(nullptr) {
  _gps = new TinyGPSPlus();
}

gps::gps(HardwareSerial *data_stream, system_logger *logger)
    : _gps(nullptr), _data_stream(data_stream), _logger(logger) {
  _gps = new TinyGPSPlus();
}

gps::~gps() {
  _data_stream = nullptr;
  delete _gps;
  _gps = nullptr;
}

int gps::init() {
  _data_stream->begin(9600);
  return 0;
}

int gps::loop(unsigned long timeout_ms) {
  unsigned long timeout = millis() + timeout_ms;

  while (_data_stream->available() > 0) {
    if (_gps->encode(_data_stream->read())) {
      gps_global_write(this->get_data());
      uint32_t current_fix_val = _gps->sentencesWithFix();
      if (_last_fix_val == 0 && current_fix_val > 0) {
        router::send(MOD_LCD, TASK_DISPLAY_MSG_GPS_FIX, 2000);
        router::send(MOD_CFG, TASK_CONFIG_TRACK_DETECT);
      }
      _last_fix_val = current_fix_val;
    }

    if (millis() > timeout) {
      return 1;
    }
  }

  return 0;
}

gps_data gps::get_data() {
  gps_data output;

  output.altitude.age = _gps->altitude.age();
  output.altitude.valid = _gps->altitude.isValid();
  output.altitude.value = _gps->altitude.meters();

  output.lat.age = _gps->location.age();
  output.lat.valid = _gps->location.isValid();
  output.lat.value = _gps->location.lat();

  output.lng.age = _gps->location.age();
  output.lng.valid = _gps->location.isValid();
  output.lng.value = _gps->location.lng();

  output.speed.age = _gps->speed.age();
  output.speed.valid = _gps->speed.isValid();
  output.speed.value = _gps->speed.kmph();

  output.course.age = _gps->course.age();
  output.course.valid = _gps->course.isValid();
  output.course.value = _gps->course.deg();

  output.num_fix = _gps->sentencesWithFix();

  return output;
}

#undef MOD