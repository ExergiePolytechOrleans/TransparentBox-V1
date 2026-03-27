// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cmd.h"

#include <string.h>
#include "data/track_store.h"
#include "base/router.h"

char *cmd::trim_arg(char *input) {
  if (input == nullptr) {
    return nullptr;
  }

  while (*input == ' ' || *input == '\t' || *input == '\r' || *input == '\n') {
    input++;
  }

  if (*input == '\0') {
    return input;
  }

  char *end = input + strlen(input) - 1;
  while (end >= input &&
         (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) {
    *end = '\0';
    end--;
  }

  return input;
}

unsigned short cmd::split_args(char *input, char *argv[], unsigned short max_args) {
  unsigned short argc = 0;
  char *p = input;
  char *token_start = input;

  if (input == nullptr || argv == nullptr || max_args == 0) {
    return 0;
  }

  if (*input == '\0') {
    return 0;
  }

  while (argc < max_args) {
    if (*p == ',' || *p == '\0') {
      char separator = *p;
      *p = '\0';

      argv[argc] = trim_arg(token_start);
      argc++;

      if (separator == '\0') {
        break;
      }

      token_start = p + 1;
    }

    p++;
  }

  return argc;
}

cmd::command_id cmd::parse_command_name(const char *input) {
  if (input == nullptr) {
    return CMD_UNKNOWN;
  }

  if (strcmp(input, "REBOOT") == 0) {
    return CMD_REBOOT;
  }

  if (strcmp(input, "DUMPCFG") == 0) {
    return CMD_DUMPCFG;
  }

  if (strcmp(input, "TRACK_LOAD") == 0) {
    return CMD_LOAD_TRACK;
  }

  return CMD_UNKNOWN;
}

int cmd::dispatch_command(command_id command, unsigned short argc, char *argv[]) {
  switch (command) {
    case CMD_REBOOT:
#ifdef INFO
      if (_logger != nullptr) {
        _logger->info("Rebooting");
      }
#endif
      delay(200);
      wdt_enable(WDTO_15MS);
      while (true) {
      }

    case CMD_DUMPCFG:
#ifdef INFO
      if (_logger != nullptr) {
        _logger->dump_config();
      }
#endif
      return 0;
      
    case CMD_LOAD_TRACK:
      if (argc != 7) {
      #ifdef ERROR
        if (_logger != nullptr) {
          _logger->error("TRACK_LOAD expects 6 arguments");
        }
      #endif
        return 1;  
      }
      track_data new_track;

      new_track.id = strtoul(argv[1], nullptr, 10);

      strncpy(new_track.name, argv[2], sizeof(new_track.name) - 1);
      new_track.name[sizeof(new_track.name) - 1] = '\0';
      
      lat_lng pt_a;
      pt_a.lat = strtod(argv[3], nullptr);
      pt_a.lng = strtod(argv[4], nullptr);
      new_track.pt_a = pt_a;

      lat_lng pt_b;
      pt_b.lat = strtod(argv[5], nullptr);
      pt_b.lng = strtod(argv[6], nullptr);
      new_track.pt_b = pt_b;
      
      if (new_track.id < 1 || new_track.id > 8) {
        #ifdef ERROR
        if (_logger != nullptr) {
          _logger->error(String("ID out of range: ") + String(new_track.id));
        }
        #endif
        return 1;
      }

      #ifdef INFO
      if (_logger != nullptr) {
        _logger->info("Loading new track");
        _logger->info(String("ID: ") + String(new_track.id));
        _logger->info(String("Name: ") + new_track.name);
        _logger->info(String("Point A lat: ") + String(new_track.pt_a.lat));
        _logger->info(String("Point A lng: ") + String(new_track.pt_a.lat));
        _logger->info(String("Point B lat: ") + String(new_track.pt_b.lat));
        _logger->info(String("Point B lng: ") + String(new_track.pt_b.lat));
      }
      #endif
      
      track_temp_global_write(new_track);
      router::send(MOD_CFG, TASK_CONFIG_WRITE_TEMP_TRACK);
       
      return 0;

    case CMD_UNKNOWN:
    default:
#ifdef ERROR
      if (_logger != nullptr) {
        if (argc > 0 && argv != nullptr && argv[0] != nullptr && argv[0][0] != '\0') {
          _logger->error(String("Unknown command: ") + String(argv[0]));
        } else {
          _logger->error("Unknown command");
        }
      }
#endif
      return 1;
  }
}

int cmd::try_parse() {
#ifdef DEBUG
  if (_logger != nullptr) {
    _logger->debug("Attempting to parse command");
  }
#endif

  char *argvp[_max_args];
  unsigned short argc = split_args(_buffer, argvp, _max_args);

  if (argc == 0 || argvp[0] == nullptr || argvp[0][0] == '\0') {
#ifdef ERROR
    if (_logger != nullptr) {
      _logger->error("Empty command");
    }
#endif
    return 1;
  }

  command_id command = parse_command_name(argvp[0]);
  return dispatch_command(command, argc, argvp);
}

int cmd::push(const Task &task) {
  return _queue.push(task);
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
  unsigned long start = millis();

  while (_data_stream->available()) {
    if ((unsigned long)(millis() - start) >= timeout_ms) {
      return 1;
    }

    char c = _data_stream->read();

    if (c == '<') {
      _buf_open = true;
      _idx = 0;
      continue;
    }

    if (!_buf_open) {
      continue;
    }

    if (c == '>') {
      _buffer[_idx] = '\0';
      _buf_open = false;
      return this->try_parse();
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

    _buffer[_idx] = c;
    _idx++;
  }

  return 0;
}