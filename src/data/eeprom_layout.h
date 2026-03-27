// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <inttypes.h>

namespace eeprom_layout {

static const uint16_t total_bytes = 4096;
static const uint16_t config_addr = 0;
static const uint16_t config_reserved_bytes = 256;
static const uint16_t track_slots = 8;
static const uint16_t track_slot_bytes = 128;
static const uint16_t track_base_addr = config_addr + config_reserved_bytes;
static const uint16_t track_end_addr = track_base_addr + (track_slots * track_slot_bytes);
static const uint16_t free_after_tracks = total_bytes - track_end_addr;

static_assert(track_end_addr <= total_bytes, "EEPROM layout exceeds physical storage");

inline uint16_t track_slot_addr(uint8_t idx) {
  return track_base_addr + ((idx - 1) * track_slot_bytes);
}

} // namespace eeprom_layout
