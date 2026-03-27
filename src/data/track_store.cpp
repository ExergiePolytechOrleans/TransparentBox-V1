// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "data/track_store.h"

 volatile track_data track_data_global = {};
volatile track_data track_data_temp_global = {};

 void track_global_read(track_data& out) {
     copy_from_volatile(out, track_data_global);
 }

 int track_global_read(unsigned short idx, track_data& out) {
     if (idx < 1 || idx > 8) {
        return 1;
     }
     track_data temp;
     EEPROM.get(idx, temp);
     if (temp.magic != CONFIG_MAGIC) {
        return 1;
     }
     out = temp;
     return 0;
 }

 void track_global_write(const track_data& in) {
     copy_to_volatile(track_data_global, in);
 }

 void track_temp_global_read(track_data& out) {
     copy_from_volatile(out, track_data_temp_global);
 }

 void track_temp_global_write(const track_data& in) {
     copy_to_volatile(track_data_temp_global, in);
 }
