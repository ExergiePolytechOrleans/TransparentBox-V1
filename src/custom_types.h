#pragma once
#include <inttypes.h>

#define CONFIG_MAGIC 0xBEEF
struct vehicle_config{
    uint16_t magic = CONFIG_MAGIC;
    bool auto_detect_track = true;
    uint8_t num_tracks = 0;
    uint8_t track_fallback = 0;
};

struct lat_lng {
    double lat;
    double lng;
};

struct track_data {
    uint16_t magic = CONFIG_MAGIC;
    int id;
    char name[32];
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
};

inline void gps_sub_copy_from_volatile(gps_sub_data& dst, const volatile gps_sub_data& src) {
    dst.age   = src.age;
    dst.valid = src.valid;
    dst.value = src.value;
}

inline void gps_sub_copy_to_volatile(volatile gps_sub_data& dst, const gps_sub_data& src) {
    dst.age   = src.age;
    dst.valid = src.valid;
    dst.value = src.value;
}

inline void gps_copy_from_volatile(gps_data& dst, const volatile gps_data& src) {
    gps_sub_copy_from_volatile(dst.altitude, src.altitude);
    gps_sub_copy_from_volatile(dst.lat,      src.lat);
    gps_sub_copy_from_volatile(dst.lng,      src.lng);
    gps_sub_copy_from_volatile(dst.speed,    src.speed);
    gps_sub_copy_from_volatile(dst.course,   src.course);
}

inline void gps_copy_to_volatile(volatile gps_data& dst, const gps_data& src) {
    gps_sub_copy_to_volatile(dst.altitude, src.altitude);
    gps_sub_copy_to_volatile(dst.lat,      src.lat);
    gps_sub_copy_to_volatile(dst.lng,      src.lng);
    gps_sub_copy_to_volatile(dst.speed,    src.speed);
    gps_sub_copy_to_volatile(dst.course,   src.course);
}