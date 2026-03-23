#pragma once
#include "custom_types.h"
#include "flags.h"
#include "modules/logger/system_logger.h"
#include "base/task.h"
#include "base/ring_buffer.h"
#include "base/module_base.h"
#include <EEPROM.h>

class config : public module_base{
private:
  vehicle_config _config;
  system_logger *_logger;
    bool _valid_config;
    track_data _loaded_track;
  ring_buffer<Task, 16> _queue;
    int read_cfg();
    int write_cfg();
    int write_cfg(const vehicle_config& new_config);
public:
    int push(const Task& task) override;
  config();
  config(system_logger *logger);
  ~config();
  int auto_init();
};

int config::push(const Task& task) {
    return _queue.push(task);
}

config::config() : _logger(nullptr), _valid_config(true) {}
config::config(system_logger *logger) : _logger(logger), _valid_config(true) {}

config::~config() {}

int config::read_cfg() {
    EEPROM.get(0, _config); 
    return 0;
}

int config::write_cfg(const vehicle_config& new_config) {
    EEPROM.put(0, new_config);
    return 0;
}

int config::auto_init() {
    this->read_cfg();
    if (_config.magic != CONFIG_MAGIC) {
        #ifdef WARN
        if (_logger != nullptr) {
            _logger->warn("Config invalid, overwriting");
        }
        #endif
        vehicle_config clean_config;
        this->write_cfg(clean_config);
        this->read_cfg();
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
