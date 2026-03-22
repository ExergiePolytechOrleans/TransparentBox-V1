#pragma once
#include "flags.h"
#include "modules/logger/system_logger.h"
#include <LiquidCrystal_I2C.h>
class lcd {
private:
  LiquidCrystal_I2C *_display;
  system_logger *_logger = nullptr;

public:
  lcd();
  lcd(system_logger *logger);
  ~lcd();
  int init();
  int print_message(String message);
};

lcd::lcd() { _display = new LiquidCrystal_I2C(0x27, 20, 4); }

lcd::lcd(system_logger *logger) {
  _display = new LiquidCrystal_I2C(0x27, 20, 4);
  _logger = logger;
}

lcd::~lcd() {}

int lcd::init() {
  _display->init();
  _display->backlight();
  _display->clear();
  _display->setCursor(0, 0);
  return 0;
}

int lcd::print_message(String message) {
  _display->clear();
  _display->setCursor(0, 0);
  _display->print(message);
  #ifdef INFO
  if (_logger != nullptr) {
    _logger->info(message);
  }
  #endif
  return 0;
}