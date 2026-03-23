#include "gps_store.h"

 volatile gps_data gps_data_global = {};

 void gps_read(gps_data& out) {
     gps_copy_from_volatile(out, gps_data_global);
 }

 void gps_write(const gps_data& in) {
    gps_copy_to_volatile(gps_data_global, in);
 }