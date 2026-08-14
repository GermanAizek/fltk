//
// Fl_Image_Capture unit test for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Image_Capture.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Output.H>
#include "unittests.h"
#include <stdio.h>

static Fl_Image_Capture s_img_cap;
static Fl_Output *s_cap_info = 0;

static void img_snap_cb(Fl_Widget *w, void *data) {
  int id = s_img_cap.capture();
  char buf[128];
  snprintf(buf, sizeof(buf), "Captured Image Frame #%d", id);
  if (s_cap_info) s_cap_info->value(buf);
}

static Fl_Widget* create_image_capture_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();

  int y = UT_TESTAREA_Y + 10;
  s_cap_info = new Fl_Output(UT_TESTAREA_X + 90, y, UT_TESTAREA_W - 100, 26, "Capture:");
  s_cap_info->value("Ready to capture still frames");
  y += 38;

  Fl_Button *btn_snap = new Fl_Button(UT_TESTAREA_X + 10, y, 140, 28, "Capture Frame");
  btn_snap->callback(img_snap_cb);

  g->end();
  return g;
}

UnitTest image_capture_test(UT_TEST_IMAGE_CAPTURE, "Image Capture", create_image_capture_test);

// Automated TEST cases
TEST(Fl_Image_Capture, CaptureFlow) {
  Fl_Image_Capture cap;
  EXPECT_TRUE(cap.is_ready_for_capture());

  int id = cap.capture();
  EXPECT_GT(id, 0);
  EXPECT_TRUE(cap.last_image() != 0);
  EXPECT_GT(cap.last_image()->w(), 0);
  EXPECT_GT(cap.last_image()->h(), 0);

  int id2 = cap.capture_to_file("snapshot.ppm");
  EXPECT_GT(id2, id);
  return true;
}
