// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "system_logger.h"
#include "data/general_store.h"

#include <stdio.h>

SystemLogger::SystemLogger(HardwareSerial *output) { output_ = output; }

SystemLogger::~SystemLogger() {}

int SystemLogger::printMessage(String prefix, String message) {
    if (output_->availableForWrite()) {
        output_->print(millis());
        output_->print(prefix);
        output_->println(message);
        return 0;
    }
    return 1;
}

#ifdef INFO
int SystemLogger::info(String message) {
    return this->printMessage(" [INFO] ", message);
}

int SystemLogger::dumpConfig() {
    VehicleConfig config;
    configGlobalRead(config);

    char buffer[64];

    // Auto detect
    snprintf(buffer, sizeof(buffer),
        "\tAuto detect tracks: %d",
        config.auto_detect_track_
    );
    this->info(String(buffer));

    // Track fallback
    snprintf(buffer, sizeof(buffer),
        "\tTrack fallback: %d",
        config.track_fallback_
    );
    this->info(String(buffer));
    
    
    this->info("\tVBAT cal factor: " + String(config.vbat_calibration_, 6));
    this->info("\tVBAT low: " + String(config.vbat_low_, 2));
    this->info("\tTENG low: " + String(config.teng_low_, 2));
    this->info("\tTENG high: " + String(config.teng_high_, 2));

    // Track slots (one per line)
    for (size_t i = 0; i < 8; i++) {
        snprintf(buffer, sizeof(buffer),
            "\tTrack slot %d: %d",
            i + 1,
            config.track_slot_occupied_[i]
        );
        this->info(String(buffer));
    }
    
    

    return 0;
}
#endif

#ifdef WARN
int SystemLogger::warn(String message) {
    return this->printMessage(" [WARNING] ", message);
}
#endif

#ifdef ERROR
int SystemLogger::error(String message) {
    return this->printMessage(" [ERROR] ", message);
}
#endif

#ifdef DEBUG
int SystemLogger::debug(String message) {
    return this->printMessage(" [DEBUG] ", message);
}
#endif

#ifdef DEEP_DEBUG
int SystemLogger::deepDebug(String message) {
    return this->printMessage(" [DEEP_DEBUG] ", message);
}
#endif
