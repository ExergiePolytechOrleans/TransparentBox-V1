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
#include "modules/thermocouple/thermocouple.h"
#include "modules/telemetry/telemetry.h"
#include "modules/lap_counter/lap_counter.h"
#include "modules/injection_counter/injection_counter.h"


SystemLogger *logger = new SystemLogger(&Serial);

Lcd *display = new Lcd(logger);
Gps *gps_module = new Gps(&Serial2, logger);
Config *system_config = new Config(logger);
Cmd *command_handler = new Cmd(&Serial, logger);
Battery *battery_module = new Battery(logger);
Thermocouple *thermocouple_module = new Thermocouple(logger);
Telemetry *telemetry_module = new Telemetry(&Serial1, logger);
LapCounter *lap_counter_modules = new LapCounter(logger);
InjectionCounter *inj_counter_module = new InjectionCounter(logger);



void setup() {
  wdt_disable();

  module_registry[module::Lcd] = display;
  module_registry[module::Gps] = gps_module;
  module_registry[module::Config] = system_config;
  module_registry[module::Cmd] = command_handler;
  module_registry[module::Battery] = battery_module;
  module_registry[module::Thermocouple] = thermocouple_module;
  module_registry[module::Telemetry] = telemetry_module;
  module_registry[module::LapCounter] = lap_counter_modules;
  module_registry[module::InjectionCounter] = inj_counter_module;

  display->init();
  display->printMessage("Starting Initialization");
  delay(750);

  display->printMessage("Cmd Init...");
  command_handler->init();
  delay(750);
  display->printMessage("Cmd Init Complete");
  delay(750);

  display->printMessage("Config Init...");
  int result = system_config->autoInit();
  delay(750);
  if (result != 0) {
    display->printMessage("Configuration Read Failed");
  } else {
    display->printMessage("Config Init Complete");
  }
  delay(750);

  display->printMessage("GPS Init...");
  gps_module->init();
  lap_counter_modules->init();
  delay(750);
  display->printMessage("GPS Init Complete");
  delay(750);

  display->printMessage("Sensors Init...");
  battery_module->init();
  thermocouple_module->init();
  inj_counter_module->init();
  delay(750);
  display->printMessage("Sensors Init Complete");
  delay(750);

  display->printMessage("Telemetry Init...");
  telemetry_module->init();
  delay(750);
  display->printMessage("Telemetry Init Complete");
  delay(750);
  router::send(module::Lcd, task::DisplayDriverPrimary);
}

void loop() {
  gps_module->loop();
  lap_counter_modules->loop();
  display->loop();
  command_handler->parseTask();
  system_config->loop();
  battery_module->loop();
  thermocouple_module->loop();
  telemetry_module->loop();
  inj_counter_module->loop();
}
