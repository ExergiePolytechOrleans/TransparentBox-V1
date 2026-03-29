// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "data/config_store.h"

 volatile VehicleConfig config_global = {};

 void configGlobalRead(VehicleConfig& out) {
     copyFromVolatile(out, config_global);
 }

 void configGlobalWrite(const VehicleConfig& in) {
     copyToVolatile(config_global, in);
 }
