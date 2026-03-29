// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "thermocouple.h"
#include "data/general_store.h"

int Thermocouple::push(const Task &task) { return queue_.push(task); }

Thermocouple::Thermocouple() : logger_(nullptr) {}

Thermocouple::Thermocouple(SystemLogger *logger) : logger_(logger) {}

Thermocouple::~Thermocouple() {}

int Thermocouple::init() {
    thermocouple_ = new MAX6675(THERMO_SCK, THERMO_CS, THERMO_SO);
    return 0;
}

int Thermocouple::loop(unsigned long timeout_ms) {
   (void)timeout_ms;

   if (millis() > last_read_at_ + update_interval_) {
    temperature_ = thermocouple_->readCelsius();
    tengGlobalWrite(temperature_);
    last_read_at_ = millis();
   } 
   return 0;
}
