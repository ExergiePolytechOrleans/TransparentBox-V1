#pragma once

#include "custom_types.h"

extern volatile gps_data gps_data_global;

void gps_global_read(gps_data& out);
void gps_global_write(const gps_data& in);