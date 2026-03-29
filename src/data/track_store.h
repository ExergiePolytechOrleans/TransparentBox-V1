// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "custom_types.h"
#include <EEPROM.h>

extern volatile TrackData track_data_global;
extern volatile TrackData track_data_temp_global;

void trackGlobalRead(TrackData& out);
int trackGlobalRead(unsigned short idx, TrackData& out);
void trackGlobalWrite(const TrackData& in);

void trackTempGlobalRead(TrackData& out);
void trackTempGlobalWrite(const TrackData& in);
