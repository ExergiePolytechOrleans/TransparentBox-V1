// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "general_store.h"

 volatile float vbat_global = 0;
 volatile float teng_global = 0;

 void vbatGlobalRead(float& out) {
     out = vbat_global;
 }

 void vbatGlobalWrite(const float& in) {
     vbat_global = in;
 }

 void tengGlobalRead(float& out) {
     out = teng_global;
 }

 void tengGlobalWrite(const float& in) {
     teng_global = in;
 }
