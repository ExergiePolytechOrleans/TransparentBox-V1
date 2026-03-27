// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "general_store.h"

 volatile double vbat_global = 0;
 volatile double teng_global = 0;

 void vbat_global_read(double& out) {
     out = vbat_global;
 }

 void vbat_global_write(const double& in) {
     vbat_global = in;
 }

 void teng_global_read(double& out) {
     out = vbat_global;
 }

 void teng_global_write(const double& in) {
     vbat_global = in;
 }