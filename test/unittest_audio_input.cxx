//
// Fl_Audio_Input unit test for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Audio_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Output.H>
#include "unittests.h"

static Fl_Audio_Input s_ainput("Default Microphone");
static Fl_Output *s_ainput_status = 0;

static void ainput_start_cb(Fl_Widget *w, void *data) {
  s_ainput.start();
  if (s_ainput_status) s_ainput_status->value("Recording (Active)");
}

static void ainput_stop_cb(Fl_Widget *w, void *data) {
  s_ainput.stop();
  if (s_ainput_status) s_ainput_status->value("Stopped (Inactive)");
}

static Fl_Widget* create_audio_input_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();

  int y = UT_TESTAREA_Y + 10;
  Fl_Output *dev_out = new Fl_Output(UT_TESTAREA_X + 90, y, UT_TESTAREA_W - 100, 26, "Device:");
  dev_out->value(s_ainput.device());
  y += 34;

  s_ainput_status = new Fl_Output(UT_TESTAREA_X + 90, y, 180, 26, "Status:");
  s_ainput_status->value("Stopped (Inactive)");
  y += 38;

  Fl_Button *btn_start = new Fl_Button(UT_TESTAREA_X + 10, y, 90, 28, "Start Mic");
  btn_start->callback(ainput_start_cb);

  Fl_Button *btn_stop = new Fl_Button(UT_TESTAREA_X + 110, y, 90, 28, "Stop Mic");
  btn_stop->callback(ainput_stop_cb);

  g->end();
  return g;
}

UnitTest audio_input_test(UT_TEST_AUDIO_INPUT, "Audio Input", create_audio_input_test);

// Automated TEST cases
TEST(Fl_Audio_Input, ConfigAndState) {
  Fl_Audio_Input input("USB Microphone");
  EXPECT_STREQ(input.device(), "USB Microphone");
  EXPECT_TRUE(input.volume() == 1.0);
  EXPECT_TRUE(!input.is_muted());
  EXPECT_TRUE(!input.is_active());

  input.set_volume(0.6);
  EXPECT_TRUE(input.volume() == 0.6);

  input.set_muted(true);
  EXPECT_TRUE(input.is_muted());

  input.start();
  EXPECT_TRUE(input.is_active());

  input.stop();
  EXPECT_TRUE(!input.is_active());
  return true;
}
