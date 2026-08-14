//
// Fl_Video_Frame_Input unit test for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Video_Frame_Input.H>
#include <FL/Fl_Video_Sink.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Output.H>
#include "unittests.h"
#include <stdio.h>

static Fl_Video_Sink s_vfi_sink;
static Fl_Video_Frame_Input s_vf_input(&s_vfi_sink);
static Fl_Output *s_vfi_info = 0;

static void vfi_push_cb(Fl_Widget *w, void *data) {
  unsigned char frame_pixels[16 * 16 * 3] = {0};
  int ok = s_vf_input.send_video_frame(frame_pixels, 16, 16, 3);
  char buf[128];
  snprintf(buf, sizeof(buf), "Sent frame (16x16) -> Sink: %dx%d (ok: %d)",
           s_vfi_sink.video_width(), s_vfi_sink.video_height(), ok);
  if (s_vfi_info) s_vfi_info->value(buf);
}

static Fl_Widget* create_video_frame_input_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();

  int y = UT_TESTAREA_Y + 10;
  s_vfi_info = new Fl_Output(UT_TESTAREA_X + 90, y, UT_TESTAREA_W - 100, 26, "Frame In:");
  s_vfi_info->value("Ready to feed memory buffer frames");
  y += 38;

  Fl_Button *btn_push = new Fl_Button(UT_TESTAREA_X + 10, y, 160, 28, "Send Memory Frame");
  btn_push->callback(vfi_push_cb);

  g->end();
  return g;
}

UnitTest video_frame_input_test(UT_TEST_VIDEO_FRAME_INPUT, "Video Frame Input", create_video_frame_input_test);

// Automated TEST cases
TEST(Fl_Video_Frame_Input, PushMemoryBuffers) {
  Fl_Video_Sink sink;
  Fl_Video_Frame_Input input(&sink);
  EXPECT_TRUE(input.video_sink() == &sink);
  EXPECT_TRUE(input.is_ready());

  unsigned char frame_pixels[3 * 64] = {0};
  int ok = input.send_video_frame(frame_pixels, 8, 8, 3);
  EXPECT_EQ(ok, 1);
  EXPECT_EQ(sink.video_width(), 8);
  EXPECT_EQ(sink.video_height(), 8);
  return true;
}
