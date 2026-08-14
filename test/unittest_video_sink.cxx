//
// Fl_Video_Sink unit test for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Video_Sink.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Output.H>
#include "unittests.h"
#include <stdio.h>

static Fl_Video_Sink s_sink;
static Fl_Output *s_sink_info = 0;

static void sink_feed_cb(Fl_Widget *w, void *data) {
  unsigned char test_buf[8 * 8 * 3] = {0};
  s_sink.set_video_frame(test_buf, 8, 8, 3);
  char buf[128];
  snprintf(buf, sizeof(buf), "Received Frame: %dx%d", s_sink.video_width(), s_sink.video_height());
  if (s_sink_info) s_sink_info->value(buf);
}

static Fl_Widget* create_video_sink_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();

  int y = UT_TESTAREA_Y + 10;
  s_sink_info = new Fl_Output(UT_TESTAREA_X + 90, y, UT_TESTAREA_W - 100, 26, "Video Sink:");
  s_sink_info->value("Idle (No Frames)");
  y += 38;

  Fl_Button *btn_feed = new Fl_Button(UT_TESTAREA_X + 10, y, 140, 28, "Push Video Frame");
  btn_feed->callback(sink_feed_cb);

  g->end();
  return g;
}

UnitTest video_sink_test(UT_TEST_VIDEO_SINK, "Video Sink", create_video_sink_test);

// Automated TEST cases
static int g_sink_test_cb_called = 0;
static void sink_test_cb(Fl_Video_Sink *s, Fl_RGB_Image *f, void *d) {
  g_sink_test_cb_called++;
}

TEST(Fl_Video_Sink, FeedAndNotify) {
  g_sink_test_cb_called = 0;
  Fl_Video_Sink sink;
  sink.video_frame_changed_callback(sink_test_cb);

  unsigned char data[12] = {0};
  sink.set_video_frame(data, 2, 2, 3);
  EXPECT_GT(g_sink_test_cb_called, 0);
  EXPECT_EQ(sink.video_width(), 2);
  EXPECT_EQ(sink.video_height(), 2);
  EXPECT_TRUE(sink.video_frame() != 0);
  return true;
}
