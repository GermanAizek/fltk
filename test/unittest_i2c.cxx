//
// Fl_I2C test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_I2C.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <stdio.h>
#include "unittests.h"

static Fl_I2C *i2c = 0;
static Fl_Box *status_box = 0;
static Fl_Input *dev_input = 0;

static void btn_cb(Fl_Widget* w, void*) {
  if (i2c && dev_input && status_box) {
    const char *dev = dev_input->value();
    if (i2c->open(dev) == 0) {
      char buf[128];
      snprintf(buf, sizeof(buf), "Opened %s successfully.", dev);
      status_box->copy_label(buf);
      i2c->close();
    } else {
      char buf[128];
      snprintf(buf, sizeof(buf), "Failed to open %s.", dev);
      status_box->copy_label(buf);
    }
    status_box->redraw();
  }
}

static Fl_Widget* create_i2c_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  i2c = new Fl_I2C();
  
  dev_input = new Fl_Input(UT_TESTAREA_X + 80, UT_TESTAREA_Y + 10, 150, 30, "Device:");
  dev_input->value("/dev/i2c-1");
  
  status_box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 50, UT_TESTAREA_W - 20, UT_TESTAREA_H/2 - 50, "I2C Test");
  status_box->box(FL_DOWN_BOX);
  status_box->color(FL_WHITE);
  status_box->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
  
  Fl_Button *btn = new Fl_Button(UT_TESTAREA_X + 10, UT_TESTAREA_Y + UT_TESTAREA_H/2 + 10, 150, 40, "Test Open");
  btn->callback(btn_cb);
  
  grp->end();
  
  return grp;
}

UnitTest i2c_test(UT_TEST_I2C, "I2C", create_i2c_test);
