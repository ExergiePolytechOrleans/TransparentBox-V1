// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

namespace cmd_help {
static constexpr char kGlobalHelpText[] =
    "Available commands:\n"
    "  HELP\n"
    "  REBOOT\n"
    "  CFG_DUMP\n"
    "  CFG_RESET\n"
    "  TRACK_PUT,<slot>,<name>,<point_a_lat>,<point_a_lng>,<point_b_lat>,<point_b_lng>\n"
    "  TRACK_DELETE,<slot>\n"
    "  TRACK_DUMP,<slot>\n"
    "  TRACK_AUTODETECT\n"
    "  DISPLAY_GPS_DEBUG\n"
    "  DISPLAY_DRIVER_PRIMARY\n"
    "  BATTERY_CAL,<actual_voltage>\n"
    "  BATTERY_PRINT_VBAT\n"
    "  BATTERY_SET_LOW,<voltage>\n"
    "  THERMO_SET_LOW,<temperature>\n"
    "  THERMO_SET_HIGH,<temperature>\n"
    "  DEBUG_UNLOCK\n"
    "  DEBUG_LOCK\n"
    "  DBG_SEND_BLANK_LAP\n";
} // namespace cmd_help
