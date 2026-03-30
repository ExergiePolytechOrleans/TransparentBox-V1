// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "lcd.h"

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
      return true;

    default:
      return false;
  }
}

void Lcd::activateMessage(screen::LcdScreen msg_screen, unsigned long duration_ms) {
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
  this->clear();
  
  GpsData gps_data;
  gpsGlobalRead(gps_data);
  
  float vbat;
  vbatGlobalRead(vbat);
  float teng;
  tengGlobalRead(teng);

  display_->setCursor(0,0);
  this->print("GPS:");
  if (gps_data.num_fix_ != 0) {
    this->print("V");
  } else {
    this->print("X");
  }
  
  display_->setCursor(3,2);
  this->print("SPEED: ");
  this->print(gps_data.speed_.value_);
  
  display_->setCursor(0,3);
  this->print("Vbat:");
  this->print(vbat);

  display_->setCursor(10,3);
  this->print("Teng:");
  this->print(teng);
  
  return 0;
}

int Lcd::renderMsgGpsFix() {
  this->clear();
  display_->setCursor(6, 1);
  this->print("GPS INFO");
  display_->setCursor(7, 2);
  this->print("FIX OK");
  return 0;
}

int Lcd::renderMsgTrackDetectOk() {
  this->clear();
  display_->setCursor(6, 0);
  this->print("GPS INFO");
  display_->setCursor(3, 1);
  this->print("TRACK DETECTED");

  TrackData track_data;
  trackGlobalRead(track_data);

  display_->setCursor((20 - strlen(track_data.name_)) / 2, 2);
  this->print(track_data.name_);
  return 0;
}

int Lcd::renderMsgConfigNoTracks() {
  this->clear();
  display_->setCursor(4, 1);
  this->print("CONFIG INFO");
  display_->setCursor(2, 2);
  this->print("NO TRACKS LOADED");
  return 0;
}

int Lcd::renderMsgBatteryLow() {
  this->clear();
  display_->setCursor(6, 1);
  this->print("WARNING");
  display_->setCursor(6, 2);
  this->print("VBAT LOW");
  return 0;
}

int Lcd::renderMsgEngineTempLow() {
  this->clear();
  display_->setCursor(6, 1);
  this->print("WARNING");
  display_->setCursor(2, 2);
  this->print("ENGINE TEMP LOW");
  return 0;
}

int Lcd::renderMsgEngineTempHigh() {
  this->clear();
  display_->setCursor(6, 1);
  this->print("WARNING");
  display_->setCursor(2, 2);
  this->print("ENGINE TEMP HIGH");
  return 0;
}

int Lcd::push(const Task &task) {
  return queue_.push(task);
}

Lcd::Lcd()
    : display_cleared_(false),
      logger_(nullptr),
      screen_(screen::Blank),
      data_screen_(screen::Blank),
      message_screen_(screen::Blank),
      last_render_(0),
      frame_duration_(2000) {
  display_ = new LiquidCrystal_I2C(0x27, 20, 4);
}

Lcd::Lcd(SystemLogger *logger)
    : display_cleared_(false),
      logger_(logger),
      screen_(screen::Blank),
      data_screen_(screen::Blank),
      message_screen_(screen::Blank),
      last_render_(0),
      frame_duration_(2000) {
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
        data_screen_ = screen::GpsDebug;
        if (!message_active_) {
          screen_ = data_screen_;
          force_render_ = true;
        }
        break;

      case task::DisplayDriverPrimary:
        data_screen_ = screen::DriverPrimary;
        if (!message_active_) {
          screen_ = data_screen_;
          force_render_ = true;
        }
        break;

      case task::DisplayMsgGpsFix:
        activateMessage(screen::MsgGpsFix, next_task.data_);
        break;

      case task::DisplayMsgTrackDetectOk:
        activateMessage(screen::MsgTrackDetectOk, next_task.data_);
        break;

      case task::DisplayMsgConfigNoTracks:
        activateMessage(screen::MsgConfigNoTracks, next_task.data_);
        break;
        
      case task::DisplayMsgBatteryLow:
        activateMessage(screen::MsgBatteryLow, next_task.data_);
        break;
        
      case task::DisplayMsgEngineTempLow:
        activateMessage(screen::MsgEngineTempLow, next_task.data_);
        break;
        
      case task::DisplayMsgEngineTempHigh:
        activateMessage(screen::MsgEngineTempLow, next_task.data_);

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

    default:
      break;
  }

  last_render_ = now;
  force_render_ = false;
  return 1;
}

#undef MOD
