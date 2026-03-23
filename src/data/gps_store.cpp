// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "gps_store.h"

 volatile gps_data gps_data_global = {};

 void gps_global_read(gps_data& out) {
     gps_copy_from_volatile(out, gps_data_global);
 }

 void gps_global_write(const gps_data& in) {
    gps_copy_to_volatile(gps_data_global, in);
 }