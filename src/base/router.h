// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "base/task.h"
#include "base/module_base.h"
#include "base/modules.h"

namespace router {
    int send(const Task& task);
    int send(module::Id target, task::Type type, uint32_t data = 0);
}
