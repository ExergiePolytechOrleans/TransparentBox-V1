// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "lcd.h"

#include "data/general_store.h"
#include "modules/gps/gps.h"
#include <Wire.h>
#include <string.h>

#define MOD "modules/lcd/lcd.h"

void Lcd::clear() {
  if (!display_cleared_) {
    display_->clear();
    display_cleared_ = true;
  }
}

void Lcd::print(const String &msg) {
  display_->print(msg);
  display_cleared_ = false;
}

void Lcd::print(char c) {
  display_->print(c);
  display_cleared_ = false;
}

void Lcd::print(const char c[]) {
  display_->print(c);
  display_cleared_ = false;
}

void Lcd::print(float d, int digits) {
  display_->print(d, digits);
  display_cleared_ = false;
}

void Lcd::print(unsigned long l, int base) {
  display_->print(l, base);
  display_cleared_ = false;
}

void Lcd::print(long l, int base) {
  display_->print(l, base);
  display_cleared_ = false;
}

void Lcd::print(unsigned int i, int base) {
  display_->print(i, base);
  display_cleared_ = false;
}

void Lcd::print(int i, int base) {
  display_->print(i, base);
  display_cleared_ = false;
}

bool Lcd::isMessageTask(task::Type type) {
  switch (type) {
  case task::DisplayMsgGpsFix:
  case task::DisplayMsgTrackDetectOk:
  case task::DisplayMsgConfigNoTracks:
  case task::DisplayMsgBatteryLow:
  case task::DisplayMsgEngineTempLow:
  case task::DisplayMsgEngineTempHigh:
  case task::DisplayMsgLapCounterLapTime:
  case task::DisplayMsgLapCounterStart:
    return true;

  default:
    return false;
  }
}

void Lcd::activateMessage(screen::LcdScreen msg_screen,
                          unsigned long duration_ms) {
  if (duration_ms == 0) {
    duration_ms = frame_duration_;
  }

  message_screen_ = msg_screen;
  message_active_ = true;
  message_end_ = millis() + duration_ms;
  screen_ = message_screen_;
  force_render_ = true;
}

void Lcd::expireMessageIfNeeded(unsigned long now) {
  if (!message_active_) {
    return;
  }

  if ((long)(now - message_end_) >= 0) {
    message_active_ = false;
    message_screen_ = screen::Blank;
    screen_ = data_screen_;
    force_render_ = true;
    base_rendered_ = false;
  }
}

screen::LcdScreen Lcd::getActiveScreen() const {
  if (message_active_) {
    return message_screen_;
  }

  return data_screen_;
}

int Lcd::renderGpsDebug() {
  this->clear();

  GpsData gps_data;
  gpsGlobalRead(gps_data);

  display_->setCursor(0, 0);
  this->print("Alt: ");
  if (gps_data.altitude_.valid_) {
    this->print(gps_data.altitude_.value_, 5);
  } else {
    this->print("not valid");
  }

  display_->setCursor(0, 1);
  this->print("Lat: ");
  if (gps_data.lat_.valid_) {
    this->print(gps_data.lat_.value_, 5);
  } else {
    this->print("not valid");
  }

  display_->setCursor(0, 2);
  this->print("Lng: ");
  if (gps_data.lng_.valid_) {
    this->print(gps_data.lng_.value_, 5);
  } else {
    this->print("not valid");
  }

  display_->setCursor(0, 3);
  this->print("Spd: ");
  if (gps_data.speed_.valid_) {
    this->print(gps_data.speed_.value_, 5);
  } else {
    this->print("not valid");
  }

  return 0;
}

int Lcd::renderDriverPrimary() {

  GpsData gps_data;
  gpsGlobalRead(gps_data);

  float vbat;
  vbatGlobalRead(vbat);
  float teng;
  tengGlobalRead(teng);

  int line_trigger;
  gpsTriggerGlobalRead(line_trigger);

  uint16_t num_laps;
  lapCountGlobalRead(num_laps);
  
  float average_speed;
  speedAvgGlobalRead(average_speed);

  if (!base_rendered_) {
    this->clear();
    display_->setCursor(0, 0);
    this->print("GPS:");

    display_->setCursor(7, 0);
    this->print("LAPS:");

    display_->setCursor(0, 2);
    this->print("SPD:");

    display_->setCursor(10, 2);
    this->print("AVG:");

    display_->setCursor(0, 3);
    this->print("V:");

    display_->setCursor(10, 3);
    this->print("T:");
    base_rendered_ = true;
  }

  display_->setCursor(4, 0);
  if (gps_data.num_fix_ != 0) {
    this->print("Y");
  } else {
    this->print("N");
  }

  display_->setCursor(12, 0);
  if (num_laps < 10)
    this->print('0');
  this->print(num_laps, 10);

  display_->setCursor(4, 2);
  if (gps_data.speed_.value_ < 10.0)
    this->print('0');
  this->print(gps_data.speed_.value_, 1);

  display_->setCursor(14, 2);
  if (average_speed < 10.0)
    this->print('0');
  this->print(average_speed, 1);

  display_->setCursor(2, 3);
  this->print(vbat, 1);

  display_->setCursor(12, 3);
  this->print(teng, 1);

  return 0;
}

int Lcd::renderMsgGpsFix() {
  if (!base_rendered_) {
    this->clear();
    display_->setCursor(6, 1);
    this->print("GPS INFO");
    display_->setCursor(7, 2);
    this->print("FIX OK");
    base_rendered_ = true;
  }
  return 0;
}

int Lcd::renderMsgGpsTrigger() {
  if (!base_rendered_) {
    this->clear();
    display_->setCursor(6, 1);
    this->print("GPS INFO");
    display_->setCursor(4, 2);
    this->print("LINE TRIGGER");
    base_rendered_ = true;
  }
  return 0;
}

int Lcd::renderMsgTrackDetectOk() {
  if (!base_rendered_) {
    this->clear();
    display_->setCursor(6, 0);
    this->print("GPS INFO");
    display_->setCursor(3, 1);
    this->print("TRACK DETECTED");

    GlobalTrackData track_data;
    trackGlobalRead(track_data);

    display_->setCursor((20 - strlen(track_data.root_.name_)) / 2, 2);
    this->print(track_data.root_.name_);
    base_rendered_ = true;
  }
  return 0;
}

int Lcd::renderMsgConfigNoTracks() {
  if (!base_rendered_) {
    this->clear();
    display_->setCursor(4, 1);
    this->print("CONFIG INFO");
    display_->setCursor(2, 2);
    this->print("NO TRACKS LOADED");
    base_rendered_ = true;
  }
  return 0;
}

int Lcd::renderMsgBatteryLow() {
  if (!base_rendered_) {
    this->clear();
    display_->setCursor(6, 1);
    this->print("WARNING");
    display_->setCursor(6, 2);
    this->print("VBAT LOW");
    base_rendered_ = true;
  }
  return 0;
}

int Lcd::renderMsgEngineTempLow() {
  if (!base_rendered_) {
  this->clear();
  display_->setCursor(6, 1);
  this->print("WARNING");
  display_->setCursor(2, 2);
  this->print("ENGINE TEMP LOW");
  base_rendered_ = true;
  }
  return 0;
}

int Lcd::renderMsgEngineTempHigh() {
  if (!base_rendered_) {
  this->clear();
  display_->setCursor(6, 1);
  this->print("WARNING");
  display_->setCursor(2, 2);
  this->print("ENGINE TEMP HIGH");
  base_rendered_ = true;
  }
  return 0;
}

int Lcd::renderMsgLapCounterStart() {
  if (!base_rendered_) {
  this->clear();
  display_->setCursor(5, 1);
  this->print("LAP COUNTER");
  display_->setCursor(6, 2);
  this->print("STARTED");
  base_rendered_ = true;
  }
  return 0;
}

int Lcd::renderMsgLapCounterLapTime() {

  if (!base_rendered_) {
  uint32_t time_cs;
  lastLapTimeGlobalRead(time_cs);

  uint32_t minutes = (time_cs / 6000);
  uint32_t seconds = (time_cs / 100) % 60;
  uint32_t cs = time_cs % 100;

  this->clear();

  display_->setCursor(6, 1);
  this->print("LAP TIME");

  display_->setCursor(6, 2);

  if (minutes < 10)
    this->print('0');
  this->print(minutes, 10);

  this->print(':');

  if (seconds < 10)
    this->print('0');
  this->print(seconds, 10);

  this->print('.');

  if (cs < 10)
    this->print('0');
  this->print(cs, 10);
  base_rendered_ = true;
  }

  return 0;
}

int Lcd::push(const Task &task) { return queue_.push(task); }

Lcd::Lcd()
    : display_cleared_(false), logger_(nullptr), screen_(screen::Blank),
      data_screen_(screen::Blank), message_screen_(screen::Blank),
      last_render_(0), frame_duration_(2000) {
  display_ = new LiquidCrystal_I2C(0x27, 20, 4);
}

Lcd::Lcd(SystemLogger *logger)
    : display_cleared_(false), logger_(logger), screen_(screen::Blank),
      data_screen_(screen::Blank), message_screen_(screen::Blank),
      last_render_(0), frame_duration_(500) {
  display_ = new LiquidCrystal_I2C(0x27, 20, 4);
}

Lcd::~Lcd() {
  delete display_;
  display_ = nullptr;
}

int Lcd::init() {
#ifdef DEEP_DEBUG
  if (logger_ != nullptr) {
    logger_->deepDebug(String(MOD) + ": init: Begin");
  }
#endif

  display_->init();
  Wire.setClock(400000);
  display_->backlight();
  this->clear();
  display_->setCursor(0, 0);
  force_render_ = true;

#ifdef DEEP_DEBUG
  if (logger_ != nullptr) {
    logger_->deepDebug(String(MOD) + ": init: End");
  }
#endif

  return 0;
}

int Lcd::printMessage(String message) {
#ifdef DEEP_DEBUG
  if (logger_ != nullptr) {
    logger_->deepDebug(String(MOD) + ": printMessage: Begin");
  }
#endif

  String original = message;
  this->clear();

  if (message.length() > 80) {
    message = message.substring(0, 80);
  }

  String lines[4] = {"", "", "", ""};
  int lineIndex = 0;

  while (message.length() > 0 && lineIndex < 4) {
    if (message.length() <= 20) {
      lines[lineIndex++] = message;
      break;
    }

    int splitIndex = message.lastIndexOf(' ', 20);

    if (splitIndex == -1 || splitIndex == 0) {
      splitIndex = 20;
    }

    lines[lineIndex++] = message.substring(0, splitIndex);

    if (splitIndex < message.length() && message.charAt(splitIndex) == ' ') {
      message = message.substring(splitIndex + 1);
    } else {
      message = message.substring(splitIndex);
    }
  }

  int usedLines = 0;
  for (int i = 0; i < 4; i++) {
    if (lines[i].length() > 0) {
      usedLines++;
    }
  }

  int startRow = 0;
  if (usedLines == 1) {
    startRow = 1;
  } else if (usedLines == 2) {
    startRow = 1;
  } else {
    startRow = 0;
  }

  int currentRow = startRow;
  for (int i = 0; i < 4; i++) {
    if (lines[i].length() == 0) {
      continue;
    }

    int col = (20 - lines[i].length()) / 2;
    if (col < 0) {
      col = 0;
    }

    display_->setCursor(col, currentRow++);
    this->print(lines[i]);
  }

#ifdef INFO
  if (logger_ != nullptr) {
    logger_->info(original);
  }
#endif

#ifdef DEEP_DEBUG
  if (logger_ != nullptr) {
    logger_->deepDebug(String(MOD) + ": printMessage: End");
  }
#endif

  return 0;
}

int Lcd::loop(unsigned long timeout_ms) {
  unsigned long now = millis();
  unsigned long start = now;

  expireMessageIfNeeded(now);

  while (true) {
    Task next_task;
    bool have_task = false;

    if (deferred_task_valid_) {
      next_task = deferred_task_;
      deferred_task_valid_ = false;
      have_task = true;
    } else {
      if (queue_.pop(next_task) == 0) {
        have_task = true;
      }
    }

    if (!have_task) {
      break;
    }

    if (message_active_ && isMessageTask(next_task.type_)) {
      deferred_task_ = next_task;
      deferred_task_valid_ = true;
      break;
    }

    switch (next_task.type_) {
    case task::DisplayGpsDebug:
      base_rendered_ = false;
      data_screen_ = screen::GpsDebug;
      if (!message_active_) {
        screen_ = data_screen_;
        force_render_ = true;
      }
      break;

    case task::DisplayDriverPrimary:
      base_rendered_ = false;
      data_screen_ = screen::DriverPrimary;
      if (!message_active_) {
        screen_ = data_screen_;
        force_render_ = true;
      }
      break;

    case task::DisplayMsgGpsFix:
      base_rendered_ = false;
      activateMessage(screen::MsgGpsFix, next_task.data_);
      break;

    case task::DisplayMsgTrackDetectOk:
      base_rendered_ = false;
      activateMessage(screen::MsgTrackDetectOk, next_task.data_);
      break;

    case task::DisplayMsgConfigNoTracks:
      base_rendered_ = false;
      activateMessage(screen::MsgConfigNoTracks, next_task.data_);
      break;

    case task::DisplayMsgBatteryLow:
      base_rendered_ = false;
      activateMessage(screen::MsgBatteryLow, next_task.data_);
      break;

    case task::DisplayMsgEngineTempLow:
      base_rendered_ = false;
      activateMessage(screen::MsgEngineTempLow, next_task.data_);
      break;

    case task::DisplayMsgEngineTempHigh:
      base_rendered_ = false;
      activateMessage(screen::MsgEngineTempHigh, next_task.data_);
      break;

    case task::DisplayMsgLapCounterStart:
      base_rendered_ = false;
      activateMessage(screen::MsgLapCounterStart, next_task.data_);
      break;

    case task::DisplayMsgLapCounterLapTime:
      base_rendered_ = false;
      activateMessage(screen::MsgLapCounterLapTime, next_task.data_);

    default:
      break;
    }

    now = millis();
    expireMessageIfNeeded(now);

    if ((unsigned long)(now - start) >= timeout_ms) {
      break;
    }
  }

  now = millis();
  expireMessageIfNeeded(now);

  screen::LcdScreen active_screen = getActiveScreen();
  if (screen_ != active_screen) {
    screen_ = active_screen;
    force_render_ = true;
  }

  if (!force_render_ && (unsigned long)(now - last_render_) < frame_duration_) {
    return 1;
  }

  switch (screen_) {
  case screen::Blank:
    this->clear();
    break;

  case screen::GpsDebug:
    this->renderGpsDebug();
    break;

  case screen::DriverPrimary:
    this->renderDriverPrimary();
    break;

  case screen::MsgGpsFix:
    this->renderMsgGpsFix();
    break;

  case screen::MsgGpsTrigger:
    this->renderMsgGpsTrigger();
    break;

  case screen::MsgTrackDetectOk:
    this->renderMsgTrackDetectOk();
    break;

  case screen::MsgConfigNoTracks:
    this->renderMsgConfigNoTracks();
    break;

  case screen::MsgBatteryLow:
    this->renderMsgBatteryLow();
    break;

  case screen::MsgEngineTempLow:
    this->renderMsgEngineTempLow();
    break;

  case screen::MsgEngineTempHigh:
    this->renderMsgEngineTempHigh();
    break;

  case screen::MsgLapCounterStart:
    this->renderMsgLapCounterStart();
    break;

  case screen::MsgLapCounterLapTime:
    this->renderMsgLapCounterLapTime();
    break;

  default:
    break;
  }

  last_render_ = now;
  force_render_ = false;
  return 1;
}

#undef MOD
