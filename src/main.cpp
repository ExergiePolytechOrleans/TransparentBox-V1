#include "flags.h"

#include "modules/cmd/cmd.h"
#include "modules/config/config.h"
#include "modules/gps/gps.h"
#include "modules/lcd/lcd.h"
#include "modules/logger/system_logger.h"
#include <Arduino.h>
#include <avr/wdt.h>

system_logger *logger_output = new system_logger(&Serial);

lcd *driver_display = new lcd(logger_output);
gps *gps_module = new gps(&Serial2, logger_output);
config *system_config = new config(logger_output);
cmd *command_handler = new cmd(&Serial, logger_output);

void setup() {
  wdt_disable();
  driver_display->init();
  driver_display->set_gps(gps_module);
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
  driver_display->switch_screen(screen::gps_debug);
}

void loop() {
  gps_module->parse_task(500);
  driver_display->render_task();
  command_handler->parse_task(500);
}
