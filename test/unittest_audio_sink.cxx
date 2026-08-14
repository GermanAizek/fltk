//
// Fl_Audio_Sink unit test for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Audio_Sink.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Output.H>
#include "unittests.h"
#include <stdio.h>

static Fl_Audio_Sink s_asink("default", 44100, 2, 16);
static Fl_Output *s_asink_info = 0;

static void asink_start_cb(Fl_Widget *w, void *data) {
  s_asink.start();
  if (s_asink_info) s_asink_info->value("Active State");
}

static void asink_write_cb(Fl_Widget *w, void *data) {
  if (s_asink.state() == Fl_Audio_Sink::ActiveState) {
    char pcm[512] = {0};
    s_asink.write(pcm, sizeof(pcm));
    char buf[128];
    snprintf(buf, sizeof(buf), "Active (Bytes Written: %d)", s_asink.bytes_written());
    if (s_asink_info) s_asink_info->value(buf);
  }
}

static void asink_stop_cb(Fl_Widget *w, void *data) {
  s_asink.stop();
  if (s_asink_info) s_asink_info->value("Stopped State");
}

static Fl_Widget* create_audio_sink_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();

  int y = UT_TESTAREA_Y + 10;
  s_asink_info = new Fl_Output(UT_TESTAREA_X + 90, y, UT_TESTAREA_W - 100, 26, "Audio Sink:");
  s_asink_info->value("Stopped State");
  y += 38;

  Fl_Button *btn_start = new Fl_Button(UT_TESTAREA_X + 10, y, 80, 28, "Start");
  btn_start->callback(asink_start_cb);

  Fl_Button *btn_write = new Fl_Button(UT_TESTAREA_X + 100, y, 90, 28, "Write PCM");
  btn_write->callback(asink_write_cb);

  Fl_Button *btn_stop = new Fl_Button(UT_TESTAREA_X + 200, y, 80, 28, "Stop");
  btn_stop->callback(asink_stop_cb);

  g->end();
  return g;
}

UnitTest audio_sink_test(UT_TEST_AUDIO_SINK, "Audio Sink", create_audio_sink_test);

// Automated TEST cases
TEST(Fl_Audio_Sink, AudioSinkControl) {
  Fl_Audio_Sink sink("default", 48000, 2, 16);
  EXPECT_EQ((int)sink.state(), (int)Fl_Audio_Sink::StoppedState);
  EXPECT_EQ(sink.sample_rate(), 48000);
  EXPECT_EQ(sink.channels(), 2);
  EXPECT_EQ(sink.bit_depth(), 16);

  sink.start();
  EXPECT_EQ((int)sink.state(), (int)Fl_Audio_Sink::ActiveState);

  char pcm[256] = {0};
  int w = sink.write(pcm, sizeof(pcm));
  EXPECT_EQ(w, (int)sizeof(pcm));
  EXPECT_EQ(sink.bytes_written(), (int)sizeof(pcm));

  sink.suspend();
  EXPECT_EQ((int)sink.state(), (int)Fl_Audio_Sink::SuspendedState);

  sink.resume();
  EXPECT_EQ((int)sink.state(), (int)Fl_Audio_Sink::ActiveState);

  sink.stop();
  EXPECT_EQ((int)sink.state(), (int)Fl_Audio_Sink::StoppedState);
  return true;
}
