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
static const uint16_t CRC_BASE_OFFSET = 3072;
static const uint16_t CRC_SIZE = sizeof(uint16_t);
static const uint16_t CRC_SLOTS = 9;
static const uint16_t CRC_END_ADDR = CRC_BASE_OFFSET + (CRC_SLOTS * CRC_SIZE);


static_assert(TRACK_END_ADDR <= CRC_BASE_OFFSET, "EEPROM layout overlap between tracks and CRC sectors");
static_assert(TRACK_END_ADDR <= TOTAL_BYTES || CRC_END_ADDR <= TOTAL_BYTES, "EEPROM layout exceeds physical storage");

inline uint16_t trackSlotAddr(uint8_t idx) {
  return TRACK_BASE_ADDR + ((idx - 1) * TRACK_SLOT_BYTES);
}

inline uint16_t configCRCAddr() {
  return CRC_BASE_OFFSET;
}

inline uint16_t trackCRCAddr(uint8_t idx) {
  return CRC_BASE_OFFSET + (idx * CRC_SIZE);
}

} // namespace eeprom_layout
