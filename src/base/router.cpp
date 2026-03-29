// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "router.h"

namespace router {
int send(const Task &task) {
  if (task.target_ == module::All) {
    int ret = 0;
    for (size_t index = 0; index < module::Count; index++) {

      ModuleBase *module_ptr = module_registry[index];

      if (module_ptr == nullptr) {
        continue;
      }

      if (module_ptr->push(task) != 0) {
        ret = 1;
      }
    }
    return ret;
  }
  if (task.target_ >= module::Count) {
    return 1;
  }

  ModuleBase *module_ptr = module_registry[task.target_];

  if (module_ptr == nullptr) {
    return 1;
  }

  return module_ptr->push(task);
}

int send(module::Id target, task::Type type, uint32_t data) {
  Task task{target, type, data};
  return send(task);
}
} // namespace router
