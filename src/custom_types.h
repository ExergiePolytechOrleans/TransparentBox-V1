#include <inttypes.h>

#define CONFIG_MAGIC 0xBEEF
typedef struct vehicle_config{
    uint16_t magic = CONFIG_MAGIC;
    bool auto_detect_track = true;
    uint8_t num_tracks = 0;
    uint8_t selected_track = 0;
};

typedef struct track_data {
    char name[64];
    
};
