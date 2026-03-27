// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <inttypes.h>
#include <string.h>

#define CONFIG_MAGIC 0xBEEF
struct vehicle_config{
    uint16_t magic = CONFIG_MAGIC;
    bool auto_detect_track = true;
    uint8_t track_fallback = 0;
    bool track_slot_occupied[8] = {false};
    double vbat_calibration = 0;
    double vbat_low = 0;
    double teng_low = 0;
    double teng_high = 0;
};

struct lat_lng {
    double lat;
    double lng;
};

struct track_data {
    uint16_t magic = CONFIG_MAGIC;
    unsigned short id;
    char name[21];
    lat_lng pt_a;
    lat_lng pt_b;
};

struct gps_sub_data {
    uint32_t age;
    bool valid;
    double value;
};

struct gps_data {
    gps_sub_data altitude;
    gps_sub_data lat;
    gps_sub_data lng;
    gps_sub_data speed;
    gps_sub_data course;
    uint32_t num_fix;
};

template<typename T>
inline void copy_from_volatile(T& dst, const volatile T& src) {
    memcpy(&dst, (const void*)&src, sizeof(T));
}

template<typename T>
inline void copy_to_volatile(volatile T& dst, const T& src) {
    memcpy((void*)&dst, &src, sizeof(T));
}