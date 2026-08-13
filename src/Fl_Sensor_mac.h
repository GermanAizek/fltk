//
// macOS Sensor interface for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     https://www.fltk.org/COPYING.php
//
// Please see the following page on how to report bugs and issues:
//
//     https://www.fltk.org/bugs.php
//

#ifndef FL_SENSOR_MAC_H
#define FL_SENSOR_MAC_H

#ifdef __cplusplus
extern "C" {
#endif

// type matches Fl_Sensor::SensorType (0: Accelerometer, 1: Proximity, 2: Compass, 3: Magnetometer, 4: Gyroscope)
void* fl_sensor_mac_init(int type);
void fl_sensor_mac_destroy(void* ctx);
int fl_sensor_mac_start(void* ctx);
void fl_sensor_mac_stop(void* ctx);
int fl_sensor_mac_is_active(void* ctx);
void fl_sensor_mac_read(void* ctx, double* x, double* y, double* z, double* value);

#ifdef __cplusplus
}
#endif

#endif
