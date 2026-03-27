// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "system_logger.h"
#include "data/general_store.h"

#include <stdio.h>

system_logger::system_logger(HardwareSerial *output)
{
    _output = output;
}

system_logger::~system_logger()
{
}

int system_logger::print_message(String pre, String message) {
    if (_output->availableForWrite()) {
        _output->print(millis());
        _output->print(pre);
        _output->println(message);
        return 0;
    }
    return 1;
}

#ifdef INFO
int system_logger::info(String message) {
    return this->print_message(" [INFO] ", message);
}

int system_logger::dump_config() {
    vehicle_config temp;
    config_global_read(temp);

    char buffer[64];

    // Auto detect
    snprintf(buffer, sizeof(buffer),
        "\tAuto detect tracks: %d",
        temp.auto_detect_track
    );
    this->info(String(buffer));

    // Track fallback
    snprintf(buffer, sizeof(buffer),
        "\tTrack fallback: %d",
        temp.track_fallback
    );
    this->info(String(buffer));
    
    
    this->info("\tVBAT cal factor: " + String(temp.vbat_calibration, 6));

    // Track slots (one per line)
    for (size_t i = 0; i < 8; i++) {
        snprintf(buffer, sizeof(buffer),
            "\tTrack slot %d: %d",
            i + 1,
            temp.track_slot_occupied[i]
        );
        this->info(String(buffer));
    }
    
    

    return 0;
}
#endif

#ifdef WARN
int system_logger::warn(String message) {
    return this->print_message(" [WARNING] ", message);
}
#endif

#ifdef ERROR
int system_logger::error(String message) {
    return this->print_message(" [ERROR] ", message);
}
#endif

#ifdef DEBUG
int system_logger::debug(String message) {
    return this->print_message(" [DEBUG] ", message);
}
#endif

#ifdef DEEP_DEBUG
int system_logger::deep_debug(String message) {
    return this->print_message(" [DEEP_DEBUG] ", message);
}
#endif
