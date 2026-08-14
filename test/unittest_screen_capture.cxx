//
// Fl_Screen_Capture unit test for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Screen_Capture.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Output.H>
#include "unittests.h"
#include <stdio.h>

static Fl_Screen_Capture s_scap;
static Fl_Output *s_scap_info = 0;

static void scap_grab_cb(Fl_Widget *w, void *data) {
  Fl_RGB_Image *img = s_scap.capture_frame();
  if (img) {
    char buf[128];
    snprintf(buf, sizeof(buf), "Captured Screen Frame: %dx%d", img->w(), img->h());
    if (s_scap_info) s_scap_info->value(buf);
  }
}

static Fl_Widget* create_screen_capture_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();

  int y = UT_TESTAREA_Y + 10;
  s_scap_info = new Fl_Output(UT_TESTAREA_X + 90, y, UT_TESTAREA_W - 100, 26, "Screen Cap:");
  s_scap_info->value("Ready to capture screen");
  y += 38;

  Fl_Button *btn_grab = new Fl_Button(UT_TESTAREA_X + 10, y, 140, 28, "Grab Screen Frame");
  btn_grab->callback(scap_grab_cb);

  g->end();
  return g;
}

UnitTest screen_capture_test(UT_TEST_SCREEN_CAPTURE, "Screen Capture", create_screen_capture_test);

// Automated TEST cases
TEST(Fl_Screen_Capture, CaptureLifecycle) {
  Fl_Screen_Capture sc;
  EXPECT_EQ(sc.screen(), 0);
  EXPECT_TRUE(!sc.is_active());

  sc.set_screen(0);
  sc.start();
  EXPECT_TRUE(sc.is_active());

  Fl_RGB_Image *frame = sc.capture_frame();
  EXPECT_TRUE(frame != 0);
  EXPECT_GT(frame->w(), 0);
  EXPECT_GT(frame->h(), 0);

  sc.stop();
  EXPECT_TRUE(!sc.is_active());
  return true;
}
