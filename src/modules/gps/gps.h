#pragma once
#include "custom_types.h"
#include "TinyGPSPlus.h"
#include "flags.h"
#include "modules/logger/system_logger.h"

#define MOD "modules/gps/gps.h"

class gps {
private:
  TinyGPSPlus *_gps;
  HardwareSerial *_data_stream;
  system_logger *_logger;

public:
  gps(HardwareSerial *data_stream);
  gps(HardwareSerial *data_stream, system_logger *logger);
  ~gps();
  int parse_task(unsigned long timeout_ms = 500);
  int init();
  gps_data get_data();
  
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
    _gps->encode(_data_stream->read());
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

gps_data gps::get_data() {
  gps_data output;

  output.altitude.age = _gps->altitude.age();
  output.altitude.valid = _gps->altitude.isValid();
  output.altitude.value = _gps->altitude.meters();

  output.lat.age = _gps->location.age();
  output.lat.valid = _gps->location.isValid();
  output.lat.value = _gps->location.lat();

  output.lng.age = _gps->location.age();
  output.lng.valid = _gps->location.isValid();
  output.lng.value = _gps->location.lng();

  output.speed.age = _gps->speed.age();
  output.speed.valid = _gps->speed.isValid();
  output.speed.value = _gps->speed.kmph();

  output.course.age = _gps->course.age();
  output.course.valid = _gps->course.isValid();
  output.course.value = _gps->course.deg();
  
  return output;
}
#undef MOD