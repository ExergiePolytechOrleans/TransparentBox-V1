// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <inttypes.h>
#include <Arduino.h>

enum module_id : uint8_t {
    MOD_CMD,
    MOD_CFG,
    MOD_GPS,
    MOD_LCD,
    MOD_BAT,
    MOD_COUNT,
    MOD_NULL,
    MOD_ALL,
};

enum task_type : uint8_t {
    TASK_NULL,
    TASK_DISPLAY_GPS_DEBUG,
    TASK_DISPLAY_DRIVER_PRIMARY,
    TASK_DISPLAY_MSG_GPS_FIX,
    TASK_DISPLAY_MSG_TRACK_DETECT_OK,
    TASK_DISPLAY_MSG_CONFIG_NO_TRACKS,
    TASK_DISPLAY_MSG_BAT_LOW,
    TASK_CONFIG_TRACK_DETECT,
    TASK_CONFIG_WRITE_TEMP_TRACK,
    TASK_CONFIG_TRACK_DELETE,
    TASK_CONFIG_CFG_RESET,
    TASK_CONFIG_VBAT_CAL_SET,
    TASK_CONFIG_VBAT_SET_LOW,
    TASK_BATTERY_CAL,
    TASK_ALL_CONFIG_UPDATED,
};

struct Task {
    module_id target;
    task_type type;
    uint32_t data;
};
