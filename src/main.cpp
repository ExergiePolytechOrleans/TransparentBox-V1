#include "flags.h"

#include <Arduino.h>
#include "modules/lcd/lcd.h"
#include "modules/gps/gps.h"
#include "modules/logger/system_logger.h"


system_logger *logger_output = new system_logger(&Serial);

lcd *driver_display = new lcd();
gps *gps_module = new gps(&Serial2, logger_output);


void setup() {
  driver_display->init();
  driver_display->print_message("Starting Initialization");
  delay(1000);

  driver_display->print_message("Serial Init...");
  Serial.begin(115200);
  delay(500);
  driver_display->print_message("Serial Init Complete");
  delay(500);
  
  driver_display->print_message("GPS Init...");
  gps_module->init();
  delay(500);
  driver_display->print_message("GPS Init Complete");
}

void loop() {
  gps_module->parse_task(500);
}
