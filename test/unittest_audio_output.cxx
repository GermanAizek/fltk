//
// Fl_Audio_Output unit test for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Audio_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Output.H>
#include "unittests.h"

static Fl_Audio_Output s_aoutput("Default Speakers");
static Fl_Output *s_vol_out = 0;

static void aout_mute_cb(Fl_Widget *w, void *data) {
  s_aoutput.set_muted(!s_aoutput.is_muted());
  if (s_vol_out) {
    s_vol_out->value(s_aoutput.is_muted() ? "Muted" : "Active (Unmuted)");
  }
}

static Fl_Widget* create_audio_output_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();

  int y = UT_TESTAREA_Y + 10;
  Fl_Output *dev_out = new Fl_Output(UT_TESTAREA_X + 90, y, UT_TESTAREA_W - 100, 26, "Device:");
  dev_out->value(s_aoutput.device());
  y += 34;

  s_vol_out = new Fl_Output(UT_TESTAREA_X + 90, y, 180, 26, "Status:");
  s_vol_out->value("Active (Unmuted)");
  y += 38;

  Fl_Button *btn_mute = new Fl_Button(UT_TESTAREA_X + 10, y, 120, 28, "Toggle Mute");
  btn_mute->callback(aout_mute_cb);

  g->end();
  return g;
}

UnitTest audio_output_test(UT_TEST_AUDIO_OUTPUT, "Audio Output", create_audio_output_test);

// Automated TEST cases
TEST(Fl_Audio_Output, ConfigAndMute) {
  Fl_Audio_Output output("Headphones");
  EXPECT_STREQ(output.device(), "Headphones");
  EXPECT_TRUE(output.volume() == 1.0);
  EXPECT_TRUE(!output.is_muted());

  output.set_volume(0.45);
  EXPECT_TRUE(output.volume() == 0.45);

  output.set_muted(true);
  EXPECT_TRUE(output.is_muted());

  output.set_muted(false);
  EXPECT_TRUE(!output.is_muted());
  return true;
}
