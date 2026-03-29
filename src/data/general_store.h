// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

extern volatile double vbat_global;
extern volatile double teng_global;

void vbatGlobalRead(double& out);
void vbatGlobalWrite(const double& in);

void tengGlobalRead(double& out);
void tengGlobalWrite(const double& in);
