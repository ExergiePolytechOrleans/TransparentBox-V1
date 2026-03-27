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



system_logger *logger_output = new system_logger(&Serial);

lcd *driver_display = new lcd(logger_output);
gps *gps_module = new gps(&Serial2, logger_output);
config *system_config = new config(logger_output);
cmd *command_handler = new cmd(&Serial, logger_output);


void setup() {
  wdt_disable();

  modules[MOD_LCD] = driver_display;
  modules[MOD_GPS] = gps_module;
  modules[MOD_CFG] = system_config;
  modules[MOD_CMD] = command_handler;

  driver_display->init();
  driver_display->print_message("Starting Initialization");
  delay(1000);

  driver_display->print_message("Cmd Init...");
  command_handler->init();
  delay(1000);
  driver_display->print_message("Cmd Init Complete");
  delay(1000);

  driver_display->print_message("Config Init...");
  int result = system_config->auto_init();
  delay(1000);
  if (result != 0) {
    driver_display->print_message("Configuration Read Failed");
  } else {
    driver_display->print_message("Config Init Complete");
  }
  delay(1000);

  driver_display->print_message("GPS Init...");
  gps_module->init();
  delay(1000);
  driver_display->print_message("GPS Init Complete");
  delay(1000);
  router::send(MOD_LCD, TASK_DISPLAY_DRIVER_PRIMARY);
}

void loop() {
  gps_module->loop();
  driver_display->loop();
  command_handler->parse_task();
  system_config->loop();
}
