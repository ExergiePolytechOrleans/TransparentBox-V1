#pragma once
#include <inttypes.h>

#define CONFIG_MAGIC 0xBEEF
struct vehicle_config{
    uint16_t magic = CONFIG_MAGIC;
    bool auto_detect_track = true;
    uint8_t num_tracks = 0;
    uint8_t selected_track = 0;
};

struct track_data {
    char name[64];
    
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
