#pragma once
#define MOD "modules/gps/gps.h"
#include "TinyGPSPlus.h"
#include "flags.h"
#include "modules/logger/system_logger.h"

class gps {
private:
  TinyGPSPlus *_gps;
  HardwareSerial *_data_stream;
  system_logger *_logger;
  bool _lock_valid = false;
  unsigned long _last_lock = 0;

public:
  gps(HardwareSerial *data_stream);
  gps(HardwareSerial *data_stream, system_logger *logger);
  ~gps();
  int parse_task(unsigned long timeout_ms = 500);
  int init();
};

gps::gps(HardwareSerial *data_stream)
    : _data_stream(data_stream), _logger(nullptr) {
  _data_stream = data_stream;
  _gps = new TinyGPSPlus();
}

gps::gps(HardwareSerial *data_stream, system_logger *logger)
    : _data_stream(data_stream), _logger(logger) {
  _gps = new TinyGPSPlus();
}

gps::~gps() {
  _data_stream = nullptr;
  delete _gps;
}

int gps::init() {
  _data_stream->begin(9600);
  return 0;
}

int gps::parse_task(unsigned long timeout_ms) {

#ifdef DEEP_DEBUG
  if (_logger != nullptr) {
    _logger->deep_debug(String(MOD) + ": parse_task: Begin");
  }
#endif
  unsigned long timeout = millis() + timeout_ms;
#ifdef DEEP_DEBUG
  if (_logger != nullptr) {
    _logger->deep_debug(String(MOD) + ": parse_task: Entering Parse Loop");
  }
#endif
  while (_data_stream->available() > 0) {
    if (_gps->encode(_data_stream->read())) {
      _lock_valid = true;
      _last_lock = millis();
    }
    if (millis() > timeout) {
#ifdef DEEP_DEBUG
      if (_logger != nullptr) {
        _logger->deep_debug(String(MOD) + ": parse_task: Parse Loop Timed Out");
      }
#endif
      return 1;
    }
  }
  return 0;
}
#undef MOD