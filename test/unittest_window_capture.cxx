//
// Fl_Window_Capture unit test for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Window_Capture.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Output.H>
#include "unittests.h"
#include <stdio.h>

static Fl_Window_Capture s_wcap;
static Fl_Output *s_wcap_info = 0;

static void wcap_grab_cb(Fl_Widget *w, void *data) {
  Fl_RGB_Image *img = s_wcap.capture_frame();
  if (img) {
    char buf[128];
    snprintf(buf, sizeof(buf), "Captured Window Frame: %dx%d", img->w(), img->h());
    if (s_wcap_info) s_wcap_info->value(buf);
  }
}

static Fl_Widget* create_window_capture_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();

  int y = UT_TESTAREA_Y + 10;
  s_wcap_info = new Fl_Output(UT_TESTAREA_X + 90, y, UT_TESTAREA_W - 100, 26, "Window Cap:");
  s_wcap_info->value("Ready to capture target window");
  y += 38;

  Fl_Button *btn_grab = new Fl_Button(UT_TESTAREA_X + 10, y, 140, 28, "Grab Window Frame");
  btn_grab->callback(wcap_grab_cb);

  g->end();
  return g;
}

UnitTest window_capture_test(UT_TEST_WINDOW_CAPTURE, "Window Capture", create_window_capture_test);

// Automated TEST cases
TEST(Fl_Window_Capture, WindowCaptureLifecycle) {
  Fl_Double_Window win(240, 180, "Test Top Window");
  Fl_Window_Capture wc(&win);
  EXPECT_TRUE(wc.window() == &win);
  EXPECT_TRUE(!wc.is_active());

  wc.start();
  EXPECT_TRUE(wc.is_active());

  Fl_RGB_Image *frame = wc.capture_frame();
  EXPECT_TRUE(frame != 0);
  EXPECT_EQ(frame->w(), 240);
  EXPECT_EQ(frame->h(), 180);

  wc.stop();
  EXPECT_TRUE(!wc.is_active());
  return true;
}
