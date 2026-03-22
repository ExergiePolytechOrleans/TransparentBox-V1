#include <inttypes.h>
typedef struct vehicle_config{
    uint16_t config_lock;
    bool auto_detect_track;
    uint8_t num_tracks;
    uint8_t selected_track;
};

typedef struct track_data {
    char name[64];
    
};
