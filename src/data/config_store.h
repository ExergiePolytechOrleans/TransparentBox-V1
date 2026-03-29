// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "custom_types.h"
#include <EEPROM.h>

extern volatile VehicleConfig config_global;

void configGlobalRead(VehicleConfig& out);
void configGlobalWrite(const VehicleConfig& in);
