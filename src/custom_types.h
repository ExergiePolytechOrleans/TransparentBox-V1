// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <inttypes.h>
#include <string.h>

#define CONFIG_MAGIC 0xBEEF

struct VehicleConfig {
    uint16_t magic_ = CONFIG_MAGIC;
    bool auto_detect_track_ = true;
    uint8_t track_fallback_ = 0;
    bool track_slot_occupied_[8] = {false};
    float vbat_calibration_ = 0;
    float vbat_low_ = 0;
    float teng_low_ = 0;
    float teng_high_ = 0;
};

struct LatLng {
    float lat_;
    float lng_;
};

struct TrackData {
    uint16_t magic_ = CONFIG_MAGIC;
    unsigned short id_;
    char name_[21];
    LatLng point_a_;
    LatLng point_b_;
};

struct GpsSubData {
    uint32_t age_;
    bool valid_;
    float value_;
};

struct GpsData {
    GpsSubData altitude_;
    GpsSubData lat_;
    GpsSubData lng_;
    GpsSubData speed_;
    GpsSubData course_;
    uint32_t num_fix_;
};

template<typename T>
inline void copyFromVolatile(T& dst, const volatile T& src) {
    memcpy(&dst, (const void*)&src, sizeof(T));
}

template<typename T>
inline void copyToVolatile(volatile T& dst, const T& src) {
    memcpy((void*)&dst, &src, sizeof(T));
}
