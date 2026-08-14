//
// Fl_Media_Recorder unit test for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Media_Recorder.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Output.H>
#include "unittests.h"

static Fl_Media_Recorder s_recorder;
static Fl_Output *s_rec_status = 0;

static void rec_record_cb(Fl_Widget *w, void *data) {
  s_recorder.record();
  if (s_rec_status) s_rec_status->value("Recording State");
}

static void rec_pause_cb(Fl_Widget *w, void *data) {
  s_recorder.pause();
  if (s_rec_status) s_rec_status->value("Paused State");
}

static void rec_stop_cb(Fl_Widget *w, void *data) {
  s_recorder.stop();
  if (s_rec_status) s_rec_status->value("Stopped State");
}

static Fl_Widget* create_media_recorder_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();

  s_recorder.set_output_location("capture_output.mp4");

  int y = UT_TESTAREA_Y + 10;
  Fl_Output *loc_out = new Fl_Output(UT_TESTAREA_X + 90, y, UT_TESTAREA_W - 100, 26, "Target:");
  loc_out->value(s_recorder.output_location());
  y += 34;

  s_rec_status = new Fl_Output(UT_TESTAREA_X + 90, y, 160, 26, "Status:");
  s_rec_status->value("Stopped State");
  y += 38;

  Fl_Button *btn_rec = new Fl_Button(UT_TESTAREA_X + 10, y, 80, 28, "Record");
  btn_rec->callback(rec_record_cb);

  Fl_Button *btn_pause = new Fl_Button(UT_TESTAREA_X + 100, y, 80, 28, "Pause");
  btn_pause->callback(rec_pause_cb);

  Fl_Button *btn_stop = new Fl_Button(UT_TESTAREA_X + 190, y, 80, 28, "Stop");
  btn_stop->callback(rec_stop_cb);

  g->end();
  return g;
}

UnitTest media_recorder_test(UT_TEST_MEDIA_RECORDER, "Media Recorder", create_media_recorder_test);

// Automated TEST cases
TEST(Fl_Media_Recorder, StateTransitions) {
  Fl_Media_Recorder rec;
  EXPECT_EQ((int)rec.recorder_state(), (int)Fl_Media_Recorder::StoppedState);

  rec.set_output_location("test_rec.mp4");
  EXPECT_STREQ(rec.output_location(), "test_rec.mp4");

  rec.record();
  EXPECT_EQ((int)rec.recorder_state(), (int)Fl_Media_Recorder::RecordingState);

  rec.pause();
  EXPECT_EQ((int)rec.recorder_state(), (int)Fl_Media_Recorder::PausedState);

  rec.stop();
  EXPECT_EQ((int)rec.recorder_state(), (int)Fl_Media_Recorder::StoppedState);
  return true;
}
