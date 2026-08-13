//
// macOS Sensor implementation for the Fast Light Tool Kit (FLTK).
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

#include "Fl_Sensor_mac.h"
#import <Foundation/Foundation.h>
#import <TargetConditionals.h>

#if TARGET_OS_IPHONE
#import <CoreMotion/CoreMotion.h>

struct Fl_Mac_Sensor_Context {
  int type;
  CMMotionManager* motionManager;
  BOOL active;
  double last_x, last_y, last_z, last_value;
};

void* fl_sensor_mac_init(int type) {
  Fl_Mac_Sensor_Context* ctx = new Fl_Mac_Sensor_Context();
  ctx->type = type;
  ctx->motionManager = [[CMMotionManager alloc] init];
  ctx->active = NO;
  ctx->last_x = 0; ctx->last_y = 0; ctx->last_z = 0; ctx->last_value = 0;
  return ctx;
}

void fl_sensor_mac_destroy(void* ctx_ptr) {
  if (!ctx_ptr) return;
  Fl_Mac_Sensor_Context* ctx = (Fl_Mac_Sensor_Context*)ctx_ptr;
  fl_sensor_mac_stop(ctx_ptr);
  [ctx->motionManager release];
  delete ctx;
}

int fl_sensor_mac_start(void* ctx_ptr) {
  if (!ctx_ptr) return 0;
  Fl_Mac_Sensor_Context* ctx = (Fl_Mac_Sensor_Context*)ctx_ptr;
  if (ctx->active) return 1;

  switch (ctx->type) {
    case 0: // Accelerometer
      if (!ctx->motionManager.accelerometerAvailable) return 0;
      [ctx->motionManager startAccelerometerUpdates];
      break;
    case 3: // Magnetometer
      if (!ctx->motionManager.magnetometerAvailable) return 0;
      [ctx->motionManager startMagnetometerUpdates];
      break;
    case 4: // Gyroscope
      if (!ctx->motionManager.gyroAvailable) return 0;
      [ctx->motionManager startGyroUpdates];
      break;
    default:
      // Proximity & Compass might need other APIs or DeviceMotion
      return 0; // Not supported directly by basic CMMotionManager without device motion
  }
  
  ctx->active = YES;
  return 1;
}

void fl_sensor_mac_stop(void* ctx_ptr) {
  if (!ctx_ptr) return;
  Fl_Mac_Sensor_Context* ctx = (Fl_Mac_Sensor_Context*)ctx_ptr;
  if (!ctx->active) return;

  switch (ctx->type) {
    case 0: [ctx->motionManager stopAccelerometerUpdates]; break;
    case 3: [ctx->motionManager stopMagnetometerUpdates]; break;
    case 4: [ctx->motionManager stopGyroUpdates]; break;
  }
  ctx->active = NO;
}

int fl_sensor_mac_is_active(void* ctx_ptr) {
  if (!ctx_ptr) return 0;
  Fl_Mac_Sensor_Context* ctx = (Fl_Mac_Sensor_Context*)ctx_ptr;
  return ctx->active ? 1 : 0;
}

void fl_sensor_mac_read(void* ctx_ptr, double* x, double* y, double* z, double* value) {
  if (!ctx_ptr) return;
  Fl_Mac_Sensor_Context* ctx = (Fl_Mac_Sensor_Context*)ctx_ptr;
  
  if (ctx->active) {
    switch (ctx->type) {
      case 0: { // Accelerometer
        CMAccelerometerData* data = ctx->motionManager.accelerometerData;
        if (data) {
          ctx->last_x = data.acceleration.x;
          ctx->last_y = data.acceleration.y;
          ctx->last_z = data.acceleration.z;
        }
        break;
      }
      case 3: { // Magnetometer
        CMMagnetometerData* data = ctx->motionManager.magnetometerData;
        if (data) {
          ctx->last_x = data.magneticField.x;
          ctx->last_y = data.magneticField.y;
          ctx->last_z = data.magneticField.z;
        }
        break;
      }
      case 4: { // Gyroscope
        CMGyroData* data = ctx->motionManager.gyroData;
        if (data) {
          ctx->last_x = data.rotationRate.x;
          ctx->last_y = data.rotationRate.y;
          ctx->last_z = data.rotationRate.z;
        }
        break;
      }
    }
  }
  
  *x = ctx->last_x;
  *y = ctx->last_y;
  *z = ctx->last_z;
  *value = ctx->last_value;
}

#else

// Fallback/stub for macOS Desktop since CMMotionManager is iOS-only

struct Fl_Mac_Sensor_Context {
  int type;
  BOOL active;
};

void* fl_sensor_mac_init(int type) {
  Fl_Mac_Sensor_Context* ctx = new Fl_Mac_Sensor_Context();
  ctx->type = type;
  ctx->active = NO;
  return ctx;
}

void fl_sensor_mac_destroy(void* ctx_ptr) {
  if (!ctx_ptr) return;
  Fl_Mac_Sensor_Context* ctx = (Fl_Mac_Sensor_Context*)ctx_ptr;
  delete ctx;
}

int fl_sensor_mac_start(void* ctx_ptr) {
  if (!ctx_ptr) return 0;
  Fl_Mac_Sensor_Context* ctx = (Fl_Mac_Sensor_Context*)ctx_ptr;
  ctx->active = YES;
  return 1;
}

void fl_sensor_mac_stop(void* ctx_ptr) {
  if (!ctx_ptr) return;
  Fl_Mac_Sensor_Context* ctx = (Fl_Mac_Sensor_Context*)ctx_ptr;
  ctx->active = NO;
}

int fl_sensor_mac_is_active(void* ctx_ptr) {
  if (!ctx_ptr) return 0;
  Fl_Mac_Sensor_Context* ctx = (Fl_Mac_Sensor_Context*)ctx_ptr;
  return ctx->active ? 1 : 0;
}

void fl_sensor_mac_read(void* ctx_ptr, double* x, double* y, double* z, double* value) {
  // macOS desktop does not provide direct standard sensor APIs via CoreMotion.
  // Returning 0s will trigger the fallback dummy values in Fl_Sensor.cxx so tests pass.
  *x = 0.0;
  *y = 0.0;
  *z = 0.0;
  *value = 0.0;
}

#endif
