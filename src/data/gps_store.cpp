// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "gps_store.h"

 volatile GpsData gps_data_global = {};

 void gpsGlobalRead(GpsData& out) {
     copyFromVolatile(out, gps_data_global);
 }

 void gpsGlobalWrite(const GpsData& in) {
     copyToVolatile(gps_data_global, in);
 }
