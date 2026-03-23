#pragma once
#include "custom_types.h"
#include "modules/logger/system_logger.h"
#include <avr/wdt.h>

class cmd {
private:
  HardwareSerial *_data_stream;
  unsigned long _baud_rate = 115200;
  system_logger *_logger;
  char _buffer[256];
  unsigned int _idx = 0;
  bool _buf_open = false;
  int try_parse();

public:
  cmd(HardwareSerial *data_stream);
  cmd(HardwareSerial *data_stream, system_logger *logger);
  ~cmd();
  int init();
  int parse_task(unsigned long timeout_ms = 500);
};

int cmd::try_parse() {
#ifdef DEBUG
  if (_logger != nullptr) {
    _logger->debug("Attempting to parse command");
  }
#endif
  if (strcmp(_buffer, "REBOOT") == 0) {
#ifdef INFO
    if (_logger != nullptr) {
      _logger->info("Rebooting");
    }
#endif
    delay(200);
    wdt_enable(WDTO_15MS);
    while (true) {
    }
  }
  return 0;
}

cmd::cmd(HardwareSerial *data_stream)
    : _data_stream(data_stream), _logger(nullptr) {}

cmd::cmd(HardwareSerial *data_stream, system_logger *logger)
    : _data_stream(data_stream), _logger(logger) {}

cmd::~cmd() {}

int cmd::init() {
  _data_stream->begin(_baud_rate);
  return 0;
}

int cmd::parse_task(unsigned long timeout_ms) {
  unsigned long timeout = millis() + timeout_ms;
  while (_data_stream->available()) {
    char c = _data_stream->read();
    if (c == '<') {
      _buf_open = true;
      _idx = 0;
      continue;
    }
    if (_idx >= sizeof(_buffer) - 1) {
      _buf_open = false;
      _idx = 0;
#ifdef ERROR
      if (_logger != nullptr) {
        _logger->error("Command parser buffer overflow");
      }
#endif
      return 1;
    }
    if (c == '>') {
      _buffer[_idx] = '\0';
      _buf_open = false;
      return this->try_parse();
    }
    if (_buf_open) {

      _buffer[_idx] = c;
      _idx++;
    }
    if (millis() > timeout) {
      return 1;
    }
  }
  return 0;
}
