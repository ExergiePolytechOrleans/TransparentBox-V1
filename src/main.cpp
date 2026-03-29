// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "flags.h"

#include "modules/cmd/cmd.h"
#include "modules/config/config.h"
#include "modules/gps/gps.h"
#include "modules/lcd/lcd.h"
#include "modules/logger/system_logger.h"
#include <Arduino.h>
#include <avr/wdt.h>
#include "base/modules.h"
#include "base/module_base.h"
#include "base/router.h"
#include "modules/battery/battery.h"


SystemLogger *logger = new SystemLogger(&Serial);

Lcd *display = new Lcd(logger);
Gps *gps_module = new Gps(&Serial2, logger);
Config *system_config = new Config(logger);
Cmd *command_handler = new Cmd(&Serial, logger);
Battery *battery_module = new Battery(logger);


void setup() {
  wdt_disable();

  module_registry[module::Lcd] = display;
  module_registry[module::Gps] = gps_module;
  module_registry[module::Config] = system_config;
  module_registry[module::Cmd] = command_handler;
  module_registry[module::Battery] = battery_module;

  display->init();
  display->printMessage("Starting Initialization");
  delay(1000);

  display->printMessage("Cmd Init...");
  command_handler->init();
  delay(1000);
  display->printMessage("Cmd Init Complete");
  delay(1000);

  display->printMessage("Config Init...");
  int result = system_config->autoInit();
  delay(1000);
  if (result != 0) {
    display->printMessage("Configuration Read Failed");
  } else {
    display->printMessage("Config Init Complete");
  }
  delay(1000);

  display->printMessage("GPS Init...");
  gps_module->init();
  delay(1000);
  display->printMessage("GPS Init Complete");
  delay(1000);

  display->printMessage("Sensors Init...");
  battery_module->init();
  delay(1000);
  display->printMessage("Sensors Init Complete");
  delay(1000);
  router::send(module::Lcd, task::DisplayDriverPrimary);
}

void loop() {
  gps_module->loop();
  display->loop();
  command_handler->parseTask();
  system_config->loop();
  battery_module->loop();
}
