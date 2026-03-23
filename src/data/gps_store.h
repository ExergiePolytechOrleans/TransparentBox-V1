// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "custom_types.h"

extern volatile gps_data gps_data_global;

void gps_global_read(gps_data& out);
void gps_global_write(const gps_data& in);