#pragma once
#include "custom_types.h"
#include "flags.h"
#include "modules/logger/system_logger.h"
#include <EEPROM.h>

class config {
private:
  vehicle_config _config;
  system_logger *_logger;
    bool _valid_config;
public:
  config();
  config(system_logger *logger);
  ~config();
  int auto_init();
};

config::config() : _logger(nullptr), _valid_config(true) {}
config::config(system_logger *logger) : _logger(logger), _valid_config(true) {}

config::~config() {}

int config::auto_init() {
    EEPROM.get(0, _config); 
    if (_config.magic != CONFIG_MAGIC) {
        #ifdef WARN
        if (_logger != nullptr) {
            _logger->warn("Config invalid, overwriting");
        }
        #endif
        vehicle_config clean_config;
        EEPROM.put(0, clean_config);
        EEPROM.get(0, _config);
        if (_config.magic != CONFIG_MAGIC) {
            #ifdef ERROR
            if (_logger != nullptr) {
                _logger->error("Config write failed, EEPROM may be burnt");
            }
            #endif
            return 1;
        }
    }
    _valid_config = true;
    return 0;
}
