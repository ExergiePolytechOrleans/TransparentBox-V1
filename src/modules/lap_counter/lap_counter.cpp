// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "lap_counter.h"
#include "base/router.h"
#include "data/general_store.h"

int LapCounter::push(const Task &task) { return queue_.push(task); }

LapCounter::LapCounter() : logger_(nullptr) {};

LapCounter::LapCounter(SystemLogger *logger) : logger_(logger) {};

LapCounter::~LapCounter() {}