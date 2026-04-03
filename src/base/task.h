// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <inttypes.h>
#include <Arduino.h>

namespace module {

enum Id : uint8_t {
    Cmd,
    Config,
    Gps,
    Lcd,
    Battery,
    Thermocouple,
    Telemetry,
    LapCounter,
    Count,
    Null,
    All,
};

} // namespace module

namespace task {

enum Type : uint8_t {
    Null,
    DisplayGpsDebug,
    DisplayDriverPrimary,
    DisplayMsgGpsFix,
    DisplayMsgTrackDetectOk,
    DisplayMsgConfigNoTracks,
    DisplayMsgBatteryLow,
    DisplayMsgEngineTempLow,
    DisplayMsgEngineTempHigh,
    DisplayMsgLapCounterStart,
    DisplayMsgLapCounterLapTime,
    ConfigTrackDetect,
    ConfigWriteTempTrack,
    ConfigTrackDelete,
    ConfigReset,
    ConfigVbatCalSet,
    ConfigVbatSetLow,
    ConfigTengSetLow,
    ConfigTengSetHigh,
    BatteryCal,
    AllConfigUpdated,
    AllTrackLoaded,
    AllStartLineTriggered,
    AllGpsFixOk,
};

} // namespace task

struct Task {
    Task(module::Id target = module::Null,
         task::Type type = task::Null,
         uint32_t data = 0)
        : target_(target), type_(type), data_(data) {}

    module::Id target_;
    task::Type type_;
    uint32_t data_;
};
