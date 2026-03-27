// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "battery.h"

int battery::push(const Task &task) { return _queue.push(task); }

battery::battery() : _logger(nullptr) {}

battery::battery(system_logger *logger)
    : _logger(logger) {}

battery::~battery() {}