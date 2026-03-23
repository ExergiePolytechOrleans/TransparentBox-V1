#pragma once
#include <inttypes.h>
#include <Arduino.h>

enum module_id : uint8_t {
    MOD_CMD,
    MOD_CFG,
    MOD_GPS,
    MOD_LCD,
    MOD_COUNT
};

enum task_type : uint8_t {
    TASK_NONE,
    TASK_DISPLAY_GPS_DEBUG,
    TASK_CONFIG_TRACK_DETECT,
};

struct Task {
    module_id target;
    task_type type;
    uint32_t data;
};