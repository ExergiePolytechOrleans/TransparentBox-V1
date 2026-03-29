// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "data/track_store.h"
#include "data/eeprom_layout.h"

 volatile TrackData track_data_global = {};
volatile TrackData track_data_temp_global = {};

 void trackGlobalRead(TrackData& out) {
     copyFromVolatile(out, track_data_global);
 }

 int trackGlobalRead(unsigned short idx, TrackData& out) {
     if (idx < 1 || idx > 8) {
        return 1;
     }
     TrackData track_data;
     EEPROM.get(eeprom_layout::trackSlotAddr(idx), track_data);
     if (track_data.magic_ != CONFIG_MAGIC) {
        return 1;
     }
     out = track_data;
     return 0;
 }

 void trackGlobalWrite(const TrackData& in) {
     copyToVolatile(track_data_global, in);
 }

 void trackTempGlobalRead(TrackData& out) {
     copyFromVolatile(out, track_data_temp_global);
 }

 void trackTempGlobalWrite(const TrackData& in) {
     copyToVolatile(track_data_temp_global, in);
 }
