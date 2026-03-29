// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <inttypes.h>

namespace eeprom_layout {

static const uint16_t TOTAL_BYTES = 4096;
static const uint16_t CONFIG_ADDR = 0;
static const uint16_t CONFIG_RESERVED_BYTES = 256;
static const uint16_t TRACK_SLOTS = 8;
static const uint16_t TRACK_SLOT_BYTES = 128;
static const uint16_t TRACK_BASE_ADDR = CONFIG_ADDR + CONFIG_RESERVED_BYTES;
static const uint16_t TRACK_END_ADDR = TRACK_BASE_ADDR + (TRACK_SLOTS * TRACK_SLOT_BYTES);
static const uint16_t FREE_AFTER_TRACKS = TOTAL_BYTES - TRACK_END_ADDR;

static_assert(TRACK_END_ADDR <= TOTAL_BYTES, "EEPROM layout exceeds physical storage");

inline uint16_t trackSlotAddr(uint8_t idx) {
  return TRACK_BASE_ADDR + ((idx - 1) * TRACK_SLOT_BYTES);
}

} // namespace eeprom_layout
