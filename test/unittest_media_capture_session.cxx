//
// Fl_Media_Capture_Session unit test for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Media_Capture_Session.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Group.H>
#include "unittests.h"

static Fl_Media_Capture_Session s_session;

static Fl_Widget* create_media_capture_session_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();

  int y = UT_TESTAREA_Y + 10;
  Fl_Output *info = new Fl_Output(UT_TESTAREA_X + 10, y, UT_TESTAREA_W - 20, 30);
  info->value("Media Capture Pipeline Coordinator (Inputs -> Outputs)");
  y += 40;

  Fl_Output *camera_status = new Fl_Output(UT_TESTAREA_X + 110, y, UT_TESTAREA_W - 120, 26, "Camera In:");
  camera_status->value("Connected");
  y += 32;

  Fl_Output *audio_status = new Fl_Output(UT_TESTAREA_X + 110, y, UT_TESTAREA_W - 120, 26, "Audio In:");
  audio_status->value("Ready");
  y += 32;

  Fl_Output *sink_status = new Fl_Output(UT_TESTAREA_X + 110, y, UT_TESTAREA_W - 120, 26, "Video Out:");
  sink_status->value("Fl_Video_Widget Active");

  g->end();
  return g;
}

UnitTest media_capture_session_test(UT_TEST_MEDIA_CAPTURE_SESSION, "Media Capture Session", create_media_capture_session_test);

// Automated TEST cases
TEST(Fl_Media_Capture_Session, SessionConnections) {
  Fl_Media_Capture_Session session;
  Fl_Camera camera(0, 0, 100, 100);
  Fl_Audio_Input audio_in("Mic");
  Fl_Screen_Capture screen_cap;
  Fl_Window_Capture window_cap;
  Fl_Image_Capture img_cap;
  Fl_Media_Recorder recorder;
  Fl_Video_Widget video_w(0, 0, 320, 240);
  Fl_Audio_Output audio_out("Speaker");

  session.set_camera(&camera);
  session.set_audio_input(&audio_in);
  session.set_screen_capture(&screen_cap);
  session.set_window_capture(&window_cap);
  session.set_image_capture(&img_cap);
  session.set_recorder(&recorder);
  session.set_video_output(&video_w);
  session.set_audio_output(&audio_out);

  EXPECT_TRUE(session.camera() == &camera);
  EXPECT_TRUE(session.audio_input() == &audio_in);
  EXPECT_TRUE(session.screen_capture() == &screen_cap);
  EXPECT_TRUE(session.window_capture() == &window_cap);
  EXPECT_TRUE(session.image_capture() == &img_cap);
  EXPECT_TRUE(session.recorder() == &recorder);
  EXPECT_TRUE(session.video_output() == &video_w);
  EXPECT_TRUE(session.video_sink() == video_w.video_sink());
  EXPECT_TRUE(session.audio_output() == &audio_out);
  return true;
}
