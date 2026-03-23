#pragma once
#include "flags.h"
#include "modules/logger/system_logger.h"
#include "modules/gps/gps.h"
#include <LiquidCrystal_I2C.h>
#include "base/task.h"
#include "base/ring_buffer.h"
#include "base/module_base.h"
#define MOD "modules/lcd/lcd.h"

namespace screen
{
  
typedef enum lcd_screen {
  blank, 
  gps_debug,
};
} // namespace screen

class lcd : public module_base{
private:
  LiquidCrystal_I2C *_display;
  bool _dispaly_cleared;
  system_logger *_logger = nullptr;
  screen::lcd_screen _screen;
  unsigned long _last_render;
  unsigned long _frame_duration;
  ring_buffer<Task, 16> _queue;
  void clear();
  void print(const String& msg);
  void print(char);
  void print(const char []);
  void print(double val, int digits);
  void print(unsigned long val, int base);
  void print(long val, int base);
  void print(unsigned int val, int base);
  void print(int val, int base);
  
  int render_gps_debug();
public:
  int push(const Task& task) override;
  lcd();
  lcd(system_logger *logger);
  ~lcd();
  int init();
  int print_message(String message);
  int loop(unsigned long timeout_ms=500);
};

void lcd::clear() {
  if (!_dispaly_cleared) {
    _display->clear();
    _dispaly_cleared = true;
  }
}

void lcd::print(const String& msg) {
    _display->print(msg);
    _dispaly_cleared = false;
}

void lcd::print(char c) {
    _display->print(c);
    _dispaly_cleared = false;
}

void lcd::print(const char c[]) {
    _display->print(c);
    _dispaly_cleared = false;
}

void lcd::print(double d, int digits=2) {
    _display->print(d, digits);
    _dispaly_cleared = false;
}

void lcd::print(unsigned long l, int base=10) {
    _display->print(l, base);
    _dispaly_cleared = false;
}

void lcd::print(long l, int base=10) {
    _display->print(l, base);
    _dispaly_cleared = false;
}

void lcd::print(unsigned int i, int base=10) {
    _display->print(i, base);
    _dispaly_cleared = false;
}

void lcd::print(int i, int base=10) {
    _display->print(i, base);
    _dispaly_cleared = false;
}

int lcd::render_gps_debug() {
  
    this->clear();
    gps_data data;
    gps_global_read(data);

    _display->setCursor(0,0);
    this->print("Alt: ");
    if (data.altitude.valid) {
      this->print(data.altitude.value, 5);
    } else {
      this->print("not valid");
    }

    _display->setCursor(0,1);
    this->print("Lat: ");
    if (data.lat.valid) {
      this->print(data.lat.value, 5);
    } else {
      this->print("not valid");
    }

    _display->setCursor(0,2);
    this->print("Lng: ");
    if (data.lng.valid) {
      this->print(data.lng.value, 5);
    } else {
      this->print("not valid");
    }

    _display->setCursor(0,3);
    this->print("Spd: ");
    if (data.speed.valid) {
      this->print(data.speed.value, 5);
    } else {
      this->print("not valid");
    }
}

int lcd::push(const Task& task) {
  return _queue.push(task);
}

lcd::lcd(): _logger(nullptr), _screen(screen::blank), _last_render(0), _frame_duration(500), _dispaly_cleared(false), _gps(nullptr) { _display = new LiquidCrystal_I2C(0x27, 20, 4); }

lcd::lcd(system_logger *logger): _logger(logger), _screen(screen::blank), _last_render(0), _frame_duration(500), _dispaly_cleared(false), _gps(nullptr)  {
  _display = new LiquidCrystal_I2C(0x27, 20, 4);

}

lcd::~lcd() {}

int lcd::init() {
#ifdef DEEP_DEBUG
  if (_logger != nullptr) {
    _logger->deep_debug(String(MOD) + ": init: Begin");
  }
#endif
  _display->init();
  Wire.setClock(400000);
  _display->backlight();
  this->clear();
  _display->setCursor(0, 0);
#ifdef DEEP_DEBUG
  if (_logger != nullptr) {
    _logger->deep_debug(String(MOD) + ": init: End");
  }
#endif
  return 0;
}

int lcd::set_gps(gps *gps) {
  _gps = gps;
}

int lcd::print_message(String message) {
#ifdef DEEP_DEBUG
  if (_logger != nullptr) {
    _logger->deep_debug(String(MOD) + ": print_message: Begin");
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
    if (lines[i].length() > 0) usedLines++;
  }

  int startRow = 0;
  if (usedLines == 1) startRow = 1;
  else if (usedLines == 2) startRow = 1;
  else startRow = 0;

  int currentRow = startRow;
  for (int i = 0; i < 4; i++) {
    if (lines[i].length() == 0) continue;

    int col = (20 - lines[i].length()) / 2;
    if (col < 0) col = 0;

    _display->setCursor(col, currentRow++);
    this->print(lines[i]);
  }

#ifdef INFO
  if (_logger != nullptr) {
    _logger->info(original);
  }
#endif

#ifdef DEEP_DEBUG
  if (_logger != nullptr) {
    _logger->deep_debug(String(MOD) + ": print_message: End");
  }
#endif

  return 0;
}

int lcd::loop(unsigned long timeout_ms) {
  unsigned long now = millis();
  unsigned long task_timeout = now + timeout_ms;
  while (_queue.size() > 0) {
    Task next_task;
    int res = _queue.pop(next_task);
    if (res != 0 ) {
      if (millis() > task_timeout) {
        break;
      }
      continue;
    }
    switch (next_task.type)
    {
    case TASK_DISPLAY_GPS_DEBUG:
      _screen = screen::gps_debug;
      break;
    
    default:
      break;
    }
    if (millis() > task_timeout) {
      break;
    }

  }
  if (now < _last_render + _frame_duration) {
    return 1;
  }
  switch (_screen)
  {
  case screen::blank:
    this->clear();
    break;
    
  case screen::gps_debug:
    this->render_gps_debug();
    break;
  
  default:
    break;
  }
  _last_render = millis();
  return 1;
}

#undef MOD