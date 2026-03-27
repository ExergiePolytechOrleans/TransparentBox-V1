// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "data/config_store.h"

 volatile vehicle_config config_global = {};

 void config_global_read(vehicle_config& out) {
     copy_from_volatile(out, config_global);
 }

 void track_global_write(const vehicle_config& in) {
     copy_to_volatile(config_global, in);
 }