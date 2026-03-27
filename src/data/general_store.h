// Copyright (C) 2026 Hector van der Aa <hector@h3cx.dev>
// Copyright (C) 2026 Association Exergie <association.exergie@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

extern volatile double vbat_global;
extern volatile double teng_global;

void vbat_global_read(double& out);
void vbat_global_write(const double& in);

void teng_global_read(double& out);
void teng_global_write(const double& in);