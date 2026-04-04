// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "injection_counter.h"
#include "base/router.h"
#include "data/general_store.h"

int InjectionCounter::push(const Task &task) { return queue_.push(task); }

InjectionCounter::InjectionCounter() : logger_(nullptr) {};

InjectionCounter::InjectionCounter(SystemLogger *logger) : logger_(logger) {};

InjectionCounter::~InjectionCounter() {};

int InjectionCounter::init() { pinMode(INJ_GPIO, INPUT); }

int InjectionCounter::loop() {
  unsigned long now = millis();

  if (now - last_check_ >= check_interval_) {
    last_check_ = now;

    int val = digitalRead(INJ_GPIO);

    if (val == 1 && last_switch_ == 0 && !waiting_debounce_) {
      waiting_debounce_ = true;
      last_switch_time_ = now;
    }

    if (waiting_debounce_) {
      if (now - last_switch_time_ >= debounce_) {
        if (digitalRead(INJ_GPIO) == 1) {
          counter_++;
          injectionCtrGlobalWrite(counter_);
        }
        waiting_debounce_ = false;
      }
    }

    last_switch_ = val;
  }

  return 0;
}