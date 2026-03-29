// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "custom_types.h"

extern volatile GpsData gps_data_global;

void gpsGlobalRead(GpsData& out);
void gpsGlobalWrite(const GpsData& in);
