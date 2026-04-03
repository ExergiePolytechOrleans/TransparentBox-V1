// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "general_store.h"

volatile float vbat_global = 0;
volatile float teng_global = 0;
volatile int gps_trigger_global = 0;
volatile uint32_t last_lap_time_global = 0;

void vbatGlobalRead(float &out) { out = vbat_global; }

void vbatGlobalWrite(const float &in) { vbat_global = in; }

void tengGlobalRead(float &out) { out = teng_global; }

void tengGlobalWrite(const float &in) { teng_global = in; }

void gpsTriggerGlobalRead(int &out) { out = gps_trigger_global; }

void gpsTriggerGlobalWrite(const int &in) { gps_trigger_global = in; }

void lastLapTimeGlobalRead(uint32_t &out) { out = last_lap_time_global; }

void lastLapTimeGlobalWrite(const uint32_t &in) { last_lap_time_global = in; }
