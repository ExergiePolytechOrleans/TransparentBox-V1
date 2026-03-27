// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "custom_types.h"
#include <EEPROM.h>

extern volatile track_data track_data_global;
extern volatile track_data track_data_temp_global;

void track_global_read(track_data& out);
int track_global_read(unsigned short idx, track_data& out);
void track_global_write(const track_data& in);

void track_temp_global_read(track_data& out);
void track_temp_global_write(const track_data& in);