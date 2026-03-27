// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "router.h"

namespace router {
int send(const Task &task) {
  if (task.target == MOD_ALL) {
    int ret = 0;
    for (size_t i = 0; i < MOD_COUNT; i++) {

      module_base *mod = modules[i];

      if (mod == nullptr) {
        continue;
      }

      if (mod->push(task) != 0) {
        ret = 1;
      }
    }
    return ret;
  }
  if (task.target >= MOD_COUNT) {
    return 1;
  }

  module_base *mod = modules[task.target];

  if (mod == nullptr) {
    return 1;
  }

  return mod->push(task);
}

int send(module_id target, task_type type, uint32_t data) {
  Task t{target, type, data};
  return send(t);
}
} // namespace router