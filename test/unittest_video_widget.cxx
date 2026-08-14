//
// Fl_Video_Widget unit test for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Video_Widget.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include "unittests.h"

static Fl_Video_Widget *s_video_w = 0;

static void vw_toggle_ar_cb(Fl_Widget *w, void *data) {
  if (s_video_w) {
    if (s_video_w->aspect_ratio_mode() == Fl_Video_Widget::KeepAspectRatio) {
      s_video_w->set_aspect_ratio_mode(Fl_Video_Widget::IgnoreAspectRatio);
    } else {
      s_video_w->set_aspect_ratio_mode(Fl_Video_Widget::KeepAspectRatio);
    }
  }
}

static void vw_feed_pattern_cb(Fl_Widget *w, void *data) {
  if (s_video_w && s_video_w->video_sink()) {
    unsigned char pattern[4 * 3] = {
      255, 0, 0,
      0, 255, 0,
      0, 0, 255,
      255, 255, 0
    };
    s_video_w->video_sink()->set_video_frame(pattern, 2, 2, 3);
  }
}

static Fl_Widget* create_video_widget_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();

  int y = UT_TESTAREA_Y + 10;
  s_video_w = new Fl_Video_Widget(UT_TESTAREA_X + 10, y, UT_TESTAREA_W - 20, 160, "Video Output Display");
  y += 170;

  Fl_Button *btn_feed = new Fl_Button(UT_TESTAREA_X + 10, y, 140, 28, "Feed Test Frame");
  btn_feed->callback(vw_feed_pattern_cb);

  Fl_Button *btn_ar = new Fl_Button(UT_TESTAREA_X + 160, y, 140, 28, "Toggle AspectRatio");
  btn_ar->callback(vw_toggle_ar_cb);

  g->end();
  return g;
}

UnitTest video_widget_test(UT_TEST_VIDEO_WIDGET, "Video Widget", create_video_widget_test);

// Automated TEST cases
TEST(Fl_Video_Widget, PropertiesAndFrame) {
  Fl_Video_Widget vw(0, 0, 320, 240, "Screen");
  EXPECT_EQ((int)vw.aspect_ratio_mode(), (int)Fl_Video_Widget::KeepAspectRatio);
  EXPECT_TRUE(!vw.is_full_screen());

  vw.set_aspect_ratio_mode(Fl_Video_Widget::IgnoreAspectRatio);
  EXPECT_EQ((int)vw.aspect_ratio_mode(), (int)Fl_Video_Widget::IgnoreAspectRatio);

  vw.set_full_screen(true);
  EXPECT_TRUE(vw.is_full_screen());

  Fl_Video_Sink *sink = vw.video_sink();
  EXPECT_TRUE(sink != 0);

  unsigned char raw[3 * 4] = {0};
  sink->set_video_frame(raw, 2, 2, 3);
  EXPECT_EQ(sink->video_width(), 2);
  EXPECT_EQ(sink->video_height(), 2);
  return true;
}
