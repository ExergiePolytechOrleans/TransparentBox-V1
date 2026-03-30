// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "custom_types.h"
#include <EEPROM.h>

extern volatile GlobalTrackData track_data_global;
extern volatile TrackData track_data_temp_global;

void trackGlobalRead(GlobalTrackData& out);
int trackGlobalRead(unsigned short idx, GlobalTrackData& out);
void trackGlobalWrite(const GlobalTrackData& in);

void trackTempGlobalRead(TrackData& out);
void trackTempGlobalWrite(const TrackData& in);
