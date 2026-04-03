// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <inttypes.h>

extern volatile float vbat_global;
extern volatile float teng_global;
extern volatile int gps_trigger_global;
extern volatile uint32_t last_lap_time_global;

void vbatGlobalRead(float& out);
void vbatGlobalWrite(const float& in);

void tengGlobalRead(float& out);
void tengGlobalWrite(const float& in);

void gpsTriggerGlobalRead(int& out);
void gpsTriggerGlobalWrite(const int& in);

void lastLapTimeGlobalRead(uint32_t& in);
void lastLapTimeGlobalWrite(const uint32_t& out);
