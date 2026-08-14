//
// Fl_Media_Player unit test for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Media_Player.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Output.H>
#include "unittests.h"
#include <stdio.h>

static Fl_Media_Player s_player;
static Fl_Output *s_status_out = 0;

static void mp_play_cb(Fl_Widget *w, void *data) {
  s_player.play();
  if (s_status_out) s_status_out->value("Playing");
}

static void mp_pause_cb(Fl_Widget *w, void *data) {
  s_player.pause();
  if (s_status_out) s_status_out->value("Paused");
}

static void mp_stop_cb(Fl_Widget *w, void *data) {
  s_player.stop();
  if (s_status_out) s_status_out->value("Stopped");
}

static Fl_Widget* create_media_player_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();

  s_player.set_source("test_stream.mp4");

  int y = UT_TESTAREA_Y + 10;
  Fl_Output *src_out = new Fl_Output(UT_TESTAREA_X + 90, y, UT_TESTAREA_W - 100, 26, "Source:");
  src_out->value(s_player.source());
  y += 34;

  s_status_out = new Fl_Output(UT_TESTAREA_X + 90, y, 150, 26, "Status:");
  s_status_out->value("Stopped");
  y += 38;

  Fl_Button *btn_play = new Fl_Button(UT_TESTAREA_X + 10, y, 80, 28, "Play");
  btn_play->callback(mp_play_cb);

  Fl_Button *btn_pause = new Fl_Button(UT_TESTAREA_X + 100, y, 80, 28, "Pause");
  btn_pause->callback(mp_pause_cb);

  Fl_Button *btn_stop = new Fl_Button(UT_TESTAREA_X + 190, y, 80, 28, "Stop");
  btn_stop->callback(mp_stop_cb);

  g->end();
  return g;
}

UnitTest media_player_test(UT_TEST_MEDIA_PLAYER, "Media Player", create_media_player_test);

// Automated TEST cases
TEST(Fl_Media_Player, InitAndDefaults) {
  Fl_Media_Player player;
  EXPECT_EQ((int)player.playback_state(), (int)Fl_Media_Player::StoppedState);
  EXPECT_EQ((int)player.media_status(), (int)Fl_Media_Player::NoMedia);
  EXPECT_EQ((int)player.error(), (int)Fl_Media_Player::NoError);
  EXPECT_EQ((int)player.duration(), 0);
  EXPECT_EQ((int)player.position(), 0);
  EXPECT_TRUE(player.volume() == 1.0);
  EXPECT_TRUE(!player.is_muted());
  return true;
}

TEST(Fl_Media_Player, WiringAndOutputs) {
  Fl_Media_Player player;
  Fl_Audio_Output audio_out("Speakers");
  Fl_Video_Widget video_widget(0, 0, 320, 240);

  player.set_audio_output(&audio_out);
  player.set_video_output(&video_widget);

  EXPECT_TRUE(player.audio_output() == &audio_out);
  EXPECT_TRUE(player.video_output() == &video_widget);
  EXPECT_TRUE(player.video_sink() == video_widget.video_sink());

  player.set_source("track.wav");
  EXPECT_STREQ(player.source(), "track.wav");
  EXPECT_EQ((int)player.media_status(), (int)Fl_Media_Player::LoadedMedia);

  player.play();
  EXPECT_EQ((int)player.playback_state(), (int)Fl_Media_Player::PlayingState);

  player.pause();
  EXPECT_EQ((int)player.playback_state(), (int)Fl_Media_Player::PausedState);

  player.stop();
  EXPECT_EQ((int)player.playback_state(), (int)Fl_Media_Player::StoppedState);
  return true;
}
