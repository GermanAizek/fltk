//
// Sensor unit tests for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Sensor.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include "unittests.h"

Fl_Widget *sensor_test_create() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  Fl_Box *b = new Fl_Box(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, 30, "Sensor tests execute core logic and print results to the console.");
  b->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
  g->end();
  return g;
}

UnitTest sensor_test(UT_TEST_SENSOR, "Sensor", sensor_test_create);

TEST(Sensor, Init) {
  Fl_Sensor sensor(Fl_Sensor::Accelerometer);
  EXPECT_EQ((int)sensor.type(), (int)Fl_Sensor::Accelerometer);
  EXPECT_TRUE(!sensor.is_active());
  return true;
}

TEST(Sensor, StartStop) {
  Fl_Sensor sensor(Fl_Sensor::Gyroscope);
  EXPECT_TRUE(sensor.start());
  EXPECT_TRUE(sensor.is_active());
  sensor.stop();
  EXPECT_TRUE(!sensor.is_active());
  return true;
}

TEST(Sensor, ReadData) {
  Fl_Sensor sensor(Fl_Sensor::Proximity);
  sensor.start();
  Fl_Sensor::SensorData data = sensor.read_data();
  // Using our dummy implementation we expect these specific non-zero values
  EXPECT_TRUE(data.value == 42.0); 
  EXPECT_TRUE(data.x == 1.0);
  return true;
}
