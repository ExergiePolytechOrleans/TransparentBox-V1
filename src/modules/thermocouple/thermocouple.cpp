// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "thermocouple.h"

int thermocouple::push(const Task &task) { return _queue.push(task); }

thermocouple::thermocouple() : _logger(nullptr) {}

thermocouple::thermocouple(system_logger *logger) : _logger(logger) {}

thermocouple::~thermocouple() {}

int thermocouple::init() {
    _thermocouple = new MAX6675(THERMO_SCK, THERMO_CS, THERMO_SO);
    return 0;
}

int thermocouple::loop(unsigned long timeout_ms) {
   if (millis() > _last_read + _update_interval) {
    _temp = _thermocouple->readCelsius();
   } 
}