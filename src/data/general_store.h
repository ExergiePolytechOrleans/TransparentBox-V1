// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

extern volatile float vbat_global;
extern volatile float teng_global;

void vbatGlobalRead(float& out);
void vbatGlobalWrite(const float& in);

void tengGlobalRead(float& out);
void tengGlobalWrite(const float& in);
