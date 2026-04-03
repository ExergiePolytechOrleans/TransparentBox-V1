// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cmd.h"

#include <string.h>
#include "data/track_store.h"
#include "data/config_store.h"
#include "data/general_store.h"
#include "base/router.h"

char *Cmd::trimArg(char *input) {
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

unsigned short Cmd::splitArgs(char *input, char *argv[], unsigned short max_args) {
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

      argv[argc] = trimArg(token_start);
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

Cmd::CommandId Cmd::parseCommandName(const char *input) {
  if (input == nullptr) {
    return Unknown;
  }

  if (strcmp(input, "REBOOT") == 0) {
    return Reboot;
  }

  if (strcmp(input, "CFG_DUMP") == 0) {
    return ConfigDump;
  }

  if (strcmp(input, "TRACK_PUT") == 0) {
    return PutTrack;
  }

  if (strcmp(input, "TRACK_DELETE") == 0) {
    return DeleteTrack;
  }

  if (strcmp(input, "TRACK_DUMP") == 0) {
    return DumpTrack;
  }

  if (strcmp(input, "CFG_RESET") == 0) {
    return ConfigReset;
  }

  if (strcmp(input, "TRACK_AUTODETECT") == 0) {
    return TrackAutodetect;
  }

  if (strcmp(input, "DISPLAY_GPS_DEBUG") == 0) {
    return DisplayGpsDebug;
  }

  if (strcmp(input, "DISPLAY_GPS_LINE_DEBUG") == 0) {
    return DisplayGpsLineDebug;
  }

  if (strcmp(input, "DISPLAY_DRIVER_PRIMARY") == 0) {
    return DisplayDriverPrimary;
  }

  if (strcmp(input, "BATTERY_CAL") == 0) {
    return BatteryCal;
  }

  if (strcmp(input, "BATTERY_PRINT_VBAT") == 0) {
    return BatteryPrintVbat;
  }

  if (strcmp(input, "BATTERY_SET_LOW") == 0) {
    return BatterySetLow;
  }

  if (strcmp(input, "THERMO_SET_LOW") == 0) {
    return ThermoSetLow;
  }

  if (strcmp(input, "THERMO_SET_HIGH") == 0) {
    return ThermoSetHigh;
  }

  return Unknown;
}

int Cmd::parseTrackSlotId(const char *id_str, unsigned short &id_out) {
  if (id_str == nullptr || id_str[0] == '\0') {
    return 1;
  }

  id_out = strtoul(id_str, nullptr, 10);
  if (id_out < 1 || id_out > 8) {
    return 1;
  }

  return 0;
}

int Cmd::dumpTrackSlot(unsigned short id) {
  VehicleConfig config;
  configGlobalRead(config);
  bool occupied = config.track_slot_occupied_[id - 1];

  GlobalTrackData track_data;
  int result = trackGlobalRead(id, track_data);
  if (result != 0) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("Track slot " + String(id) + " has no valid track data");
    }
#endif
    return 1;
  }

#ifdef INFO
  if (logger_ != nullptr) {
    logger_->info("Track dump for slot " + String(id));
    logger_->info(String("\tOccupied flag: ") + String(occupied));
    logger_->info(String("\tID: ") + String(track_data.root_.id_));
    logger_->info(String("\tName: ") + String(track_data.root_.name_));
    logger_->info(String("\tPoint A lat: ") + String(track_data.root_.point_a_.lat_, 6));
    logger_->info(String("\tPoint A lng: ") + String(track_data.root_.point_a_.lng_, 6));
    logger_->info(String("\tPoint B lat: ") + String(track_data.root_.point_b_.lat_, 6));
    logger_->info(String("\tPoint B lng: ") + String(track_data.root_.point_b_.lng_, 6));
  }
#endif

  return 0;
}

int Cmd::handleRebootCommand(unsigned short argc) {
  if (argc != 1) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("REBOOT expects no arguments");
    }
#endif
    return 1;
  }

#ifdef INFO
  if (logger_ != nullptr) {
    logger_->info("Rebooting");
  }
#endif
  delay(200);
  wdt_enable(WDTO_15MS);
  while (true) {
  }
  return 0;
}

int Cmd::handleDumpConfigCommand(unsigned short argc) {
  if (argc != 1) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("DUMPCFG expects no arguments");
    }
#endif
    return 1;
  }

#ifdef INFO
  if (logger_ != nullptr) {
    logger_->dumpConfig();
  }
#endif
  return 0;
}

int Cmd::handleTrackPutCommand(unsigned short argc, char *argv[]) {
  if (argc != 7) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("TRACK_PUT expects 6 arguments");
    }
#endif
    return 1;
  }

  TrackData new_track;
  if (parseTrackSlotId(argv[1], new_track.id_) != 0) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error(String("ID out of range: ") + String(argv[1]));
    }
#endif
    return 1;
  }

  strncpy(new_track.name_, argv[2], sizeof(new_track.name_) - 1);
  new_track.name_[sizeof(new_track.name_) - 1] = '\0';

  LatLng point_a;
  point_a.lat_ = strtod(argv[3], nullptr);
  point_a.lng_ = strtod(argv[4], nullptr);
  new_track.point_a_ = point_a;

  LatLng point_b;
  point_b.lat_ = strtod(argv[5], nullptr);
  point_b.lng_ = strtod(argv[6], nullptr);
  new_track.point_b_ = point_b;

#ifdef INFO
  if (logger_ != nullptr) {
    logger_->info("Loading new track");
    logger_->info(String("ID: ") + String(new_track.id_));
    logger_->info(String("Name: ") + new_track.name_);
    logger_->info(String("Point A lat: ") + String(new_track.point_a_.lat_));
    logger_->info(String("Point A lng: ") + String(new_track.point_a_.lng_));
    logger_->info(String("Point B lat: ") + String(new_track.point_b_.lat_));
    logger_->info(String("Point B lng: ") + String(new_track.point_b_.lng_));
  }
#endif

  trackTempGlobalWrite(new_track);
  return router::send(module::Config, task::ConfigWriteTempTrack);
}

int Cmd::handleTrackDeleteCommand(unsigned short argc, char *argv[]) {
  if (argc != 2) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("TRACK_DELETE expects 1 argument");
    }
#endif
    return 1;
  }

  unsigned short id;
  if (parseTrackSlotId(argv[1], id) != 0) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error(String("ID out of range: ") + String(argv[1]));
    }
#endif
    return 1;
  }

  return router::send(module::Config, task::ConfigTrackDelete, id);
}

int Cmd::handleTrackDumpCommand(unsigned short argc, char *argv[]) {
  if (argc != 2) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("TRACK_DUMP expects 1 argument");
    }
#endif
    return 1;
  }

  unsigned short id;
  if (parseTrackSlotId(argv[1], id) != 0) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error(String("ID out of range: ") + String(argv[1]));
    }
#endif
    return 1;
  }

  return this->dumpTrackSlot(id);
}

int Cmd::handleConfigResetCommand(unsigned short argc) {
  if (argc != 1) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("CFG_RESET expects no arguments");
    }
#endif
    return 1;
  }

#ifdef INFO
  if (logger_ != nullptr) {
    logger_->info("Resetting config");
  }
#endif
  return router::send(module::Config, task::ConfigReset);
}

int Cmd::handleTrackAutodetectCommand(unsigned short argc) {
  if (argc != 1) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("TRACK_AUTODETECT expects no arguments");
    }
#endif
    return 1;
  }

#ifdef INFO
  if (logger_ != nullptr) {
    logger_->info("Detecting track");
  }
#endif
  return router::send(module::Config, task::ConfigTrackDetect, 1);
}

int Cmd::handleDisplayGpsDebug(unsigned short argc) {
  if (argc != 1) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("DISPLAY_GPS_DEBUG expects no arguments");
    }
#endif
    return 1;
  }

#ifdef INFO
  if (logger_ != nullptr) {
    logger_->info("Switching to GPS_DEBUG display");
  }
#endif
  return router::send(module::Lcd, task::DisplayGpsDebug);
}


int Cmd::handleDisplayDriverPrimary(unsigned short argc) {
  if (argc != 1) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("DISPLAY_DRIVER_PRIMARY expects no arguments");
    }
#endif
    return 1;
  }

#ifdef INFO
  if (logger_ != nullptr) {
    logger_->info("Switching to DRIVER_PRIMARY display");
  }
#endif
  return router::send(module::Lcd, task::DisplayDriverPrimary);
}

int Cmd::handleBatteryCal(unsigned short argc, char *argv[]) {
  if (argc != 2) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("BATTERY_CAL expects 1 argument");
    }
#endif
    return 1;
  }
  float vbat = strtod(argv[1], nullptr);
  uint32_t task_data;
  memcpy(&task_data, &vbat, sizeof(uint32_t));
#ifdef INFO
  if (logger_ != nullptr) {
    logger_->info("Calibrating VBAT");
  }
#endif
  router::send(module::Battery, task::BatteryCal, task_data);
  return 0;
}

int Cmd::handleBatteryPrintVbat(unsigned short argc) {
  if (argc != 1) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("BATTERY_PRINT_VBAT expects no arguments");
    }
#endif
    return 1;
  }

#ifdef INFO
  float vbat;
  vbatGlobalRead(vbat);
  if (logger_ != nullptr) {
    logger_->info("VBAT: " + String(vbat));
  }
#endif
  return 0;
}

int Cmd::handleBatterySetLow(unsigned short argc, char* argv[]) {
  if (argc != 2) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("BATTERY_SET_LOW expects 1 argument");
    }
#endif
    return 1;
  }
  float low = strtod(argv[1], nullptr);
  uint32_t task_data;
  memcpy(&task_data, &low, sizeof(uint32_t));
#ifdef INFO
  if (logger_ != nullptr) {
    logger_->info("Setting warning level for VBAT");
  }
#endif
  router::send(module::Config, task::ConfigVbatSetLow, task_data);
  return 0;
}

int Cmd::handleThermoSetLow(unsigned short argc, char* argv[]) {
  if (argc != 2) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("THERMO_SET_LOW expects 1 argument");
    }
#endif
    return 1;
  }
  float low = strtod(argv[1], nullptr);
  uint32_t task_data;
  memcpy(&task_data, &low, sizeof(uint32_t));
#ifdef INFO
  if (logger_ != nullptr) {
    logger_->info("Setting low level for TENG");
  }
#endif
  router::send(module::Config, task::ConfigTengSetLow, task_data);
  return 0;
}

int Cmd::handleThermoSetHigh(unsigned short argc, char* argv[]) {
  if (argc != 2) {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("THERMO_SET_HIGH expects 1 argument");
    }
#endif
    return 1;
  }
  float low = strtod(argv[1], nullptr);
  uint32_t task_data;
  memcpy(&task_data, &low, sizeof(uint32_t));
#ifdef INFO
  if (logger_ != nullptr) {
    logger_->info("Setting high level for TENG");
  }
#endif
  router::send(module::Config, task::ConfigTengSetHigh, task_data);
  return 0;
}

int Cmd::handleUnknownCommand(unsigned short argc, char *argv[]) {
#ifdef ERROR
  if (logger_ != nullptr) {
    if (argc > 0 && argv != nullptr && argv[0] != nullptr && argv[0][0] != '\0') {
      logger_->error(String("Unknown command: ") + String(argv[0]));
    } else {
      logger_->error("Unknown command");
    }
  }
#endif
  return 1;
}

int Cmd::dispatchCommand(CommandId command, unsigned short argc, char *argv[]) {
  switch (command) {
    case Reboot:
      return this->handleRebootCommand(argc);

    case ConfigDump:
      return this->handleDumpConfigCommand(argc);

    case PutTrack:
      return this->handleTrackPutCommand(argc, argv);

    case DeleteTrack:
      return this->handleTrackDeleteCommand(argc, argv);

    case DumpTrack:
      return this->handleTrackDumpCommand(argc, argv);

    case ConfigReset:
      return this->handleConfigResetCommand(argc);
      
    case TrackAutodetect:
      return this->handleTrackAutodetectCommand(argc);
      
    case DisplayGpsDebug:
      return this->handleDisplayGpsDebug(argc);

    case DisplayDriverPrimary:
      return this->handleDisplayDriverPrimary(argc);
      
    case BatteryCal:
      return this->handleBatteryCal(argc, argv);
      
    case BatteryPrintVbat:
      return this->handleBatteryPrintVbat(argc);
      
    case BatterySetLow:
      return this->handleBatterySetLow(argc, argv);

    case ThermoSetLow:
      return this->handleThermoSetLow(argc, argv);

    case ThermoSetHigh:
      return this->handleThermoSetHigh(argc, argv);

    case Unknown:
    default:
      return this->handleUnknownCommand(argc, argv);
  }
}

int Cmd::tryParse() {
#ifdef DEBUG
  if (logger_ != nullptr) {
    logger_->debug("Attempting to parse command");
  }
#endif

  char *argvp[MAX_ARGS];
  unsigned short argc = splitArgs(buffer_, argvp, MAX_ARGS);

  if (argc == 0 || argvp[0] == nullptr || argvp[0][0] == '\0') {
#ifdef ERROR
    if (logger_ != nullptr) {
      logger_->error("Empty command");
    }
#endif
    return 1;
  }

  CommandId command = parseCommandName(argvp[0]);
  return dispatchCommand(command, argc, argvp);
}

int Cmd::push(const Task &task) {
  return queue_.push(task);
}

Cmd::Cmd(HardwareSerial *data_stream)
    : data_stream_(data_stream), logger_(nullptr) {}

Cmd::Cmd(HardwareSerial *data_stream, SystemLogger *logger)
    : data_stream_(data_stream), logger_(logger) {}

Cmd::~Cmd() {}

int Cmd::init() {
  data_stream_->begin(baud_rate_);
  return 0;
}

int Cmd::parseTask(unsigned long timeout_ms) {
  unsigned long start = millis();

  while (data_stream_->available()) {
    if ((unsigned long)(millis() - start) >= timeout_ms) {
      return 1;
    }

    char current_char = data_stream_->read();

    if (current_char == '<') {
      buffer_open_ = true;
      index_ = 0;
      continue;
    }

    if (!buffer_open_) {
      continue;
    }

    if (current_char == '>') {
      buffer_[index_] = '\0';
      buffer_open_ = false;
      return this->tryParse();
    }

    if (index_ >= sizeof(buffer_) - 1) {
      buffer_open_ = false;
      index_ = 0;
#ifdef ERROR
      if (logger_ != nullptr) {
        logger_->error("Command parser buffer overflow");
      }
#endif
      return 1;
    }

    buffer_[index_] = current_char;
    index_++;
  }

  return 0;
}
