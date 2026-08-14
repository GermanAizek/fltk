//
// Fl_Audio_Buffer_Input unit test for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Audio_Buffer_Input.H>
#include <FL/Fl_Audio_Sink.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Output.H>
#include "unittests.h"
#include <stdio.h>

static Fl_Audio_Sink s_abi_sink("default");
static Fl_Audio_Buffer_Input s_ab_input(&s_abi_sink);
static Fl_Output *s_abi_info = 0;

static void abi_push_cb(Fl_Widget *w, void *data) {
  s_abi_sink.start();
  char buf_data[512] = {0};
  int sent = s_ab_input.send_audio_buffer(buf_data, sizeof(buf_data));
  char buf[128];
  snprintf(buf, sizeof(buf), "Total Buffer Sent: %d bytes (last: %d)",
           s_ab_input.total_bytes_sent(), sent);
  if (s_abi_info) s_abi_info->value(buf);
}

static Fl_Widget* create_audio_buffer_input_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();

  int y = UT_TESTAREA_Y + 10;
  s_abi_info = new Fl_Output(UT_TESTAREA_X + 90, y, UT_TESTAREA_W - 100, 26, "Buffer In:");
  s_abi_info->value("Ready to send PCM audio buffers");
  y += 38;

  Fl_Button *btn_push = new Fl_Button(UT_TESTAREA_X + 10, y, 160, 28, "Push Audio Buffer");
  btn_push->callback(abi_push_cb);

  g->end();
  return g;
}

UnitTest audio_buffer_input_test(UT_TEST_AUDIO_BUFFER_INPUT, "Audio Buffer Input", create_audio_buffer_input_test);

// Automated TEST cases
TEST(Fl_Audio_Buffer_Input, PushMemoryAudio) {
  Fl_Audio_Sink sink("default");
  sink.start();

  Fl_Audio_Buffer_Input input(&sink);
  EXPECT_TRUE(input.audio_sink() == &sink);
  EXPECT_TRUE(input.is_ready());

  char audio_buf[1024] = {0};
  int sent = input.send_audio_buffer(audio_buf, sizeof(audio_buf));
  EXPECT_EQ(sent, (int)sizeof(audio_buf));
  EXPECT_EQ(input.total_bytes_sent(), (int)sizeof(audio_buf));

  input.clear();
  EXPECT_EQ(input.total_bytes_sent(), 0);
  return true;
}
