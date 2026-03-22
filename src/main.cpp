#include "flags.h"

#include "modules/config/config.h"
#include "modules/gps/gps.h"
#include "modules/lcd/lcd.h"
#include "modules/logger/system_logger.h"
#include <Arduino.h>

system_logger *logger_output = new system_logger(&Serial);

lcd *driver_display = new lcd(logger_output);
gps *gps_module = new gps(&Serial2, logger_output);
config *system_config = new config(logger_output);

void setup() {
  driver_display->init();
  driver_display->print_message("Starting Initialization");
  delay(1000);

  driver_display->print_message("Serial Init...");
  Serial.begin(115200);
  delay(500);
  driver_display->print_message("Serial Init Complete");
  delay(500);

  driver_display->print_message("Config Init...");
  int result = system_config->auto_init();
  delay(500);
  if (result != 0) {
    driver_display->print_message("Configuration Read Failed");
  } else {
    driver_display->print_message("Config Init Complete");
  }
  delay(500);

  driver_display->print_message("GPS Init...");
  gps_module->init();
  delay(500);
  driver_display->print_message("GPS Init Complete");
}

void loop() { gps_module->parse_task(500); }
