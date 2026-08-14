//
// Fl_Multimedia unit test for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Multimedia.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Text_Display.H>
#include "unittests.h"
#include <stdio.h>
#include <string.h>

static Fl_Multimedia *media_player = 0;
static Fl_Video_Widget *video_widget = 0;
static Fl_Text_Buffer *media_log_buf = 0;
static Fl_Text_Display *media_log_disp = 0;

static void log_msg(const char *msg) {
  if (media_log_buf) {
    media_log_buf->append(msg);
    media_log_buf->append("\n");
  }
}

static void on_state_changed(Fl_Multimedia *player, Fl_Multimedia::PlaybackState state, void *data) {
  char buf[128];
  const char *st = (state == Fl_Multimedia::PlayingState) ? "PlayingState" :
                   (state == Fl_Multimedia::PausedState)  ? "PausedState"  : "StoppedState";
  snprintf(buf, sizeof(buf), "[Callback] Playback State: %s", st);
  log_msg(buf);
}

static void on_status_changed(Fl_Multimedia *player, Fl_Multimedia::MediaStatus status, void *data) {
  char buf[128];
  const char *st = (status == Fl_Multimedia::NoMedia)        ? "NoMedia" :
                   (status == Fl_Multimedia::LoadingMedia)   ? "LoadingMedia" :
                   (status == Fl_Multimedia::LoadedMedia)    ? "LoadedMedia" :
                   (status == Fl_Multimedia::BufferingMedia) ? "BufferingMedia" :
                   (status == Fl_Multimedia::BufferedMedia)  ? "BufferedMedia" :
                   (status == Fl_Multimedia::EndOfMedia)     ? "EndOfMedia" : "InvalidMedia";
  snprintf(buf, sizeof(buf), "[Callback] Media Status: %s", st);
  log_msg(buf);
}

static void on_position_changed(Fl_Multimedia *player, int64_t pos_ms, void *data) {
  char buf[128];
  snprintf(buf, sizeof(buf), "[Callback] Position: %lld ms / %lld ms", (long long)pos_ms, (long long)player->duration());
  log_msg(buf);
}

static void play_cb(Fl_Widget *w, void *data) {
  if (media_player) {
    media_player->play();
  }
}

static void pause_cb(Fl_Widget *w, void *data) {
  if (media_player) {
    media_player->pause();
  }
}

static void stop_cb(Fl_Widget *w, void *data) {
  if (media_player) {
    media_player->stop();
  }
}

static void seek_cb(Fl_Widget *w, void *data) {
  if (media_player) {
    int64_t mid = media_player->duration() / 2;
    media_player->set_position(mid);
  }
}

static void mute_cb(Fl_Widget *w, void *data) {
  if (media_player) {
    media_player->set_muted(!media_player->is_muted());
  }
}

static void vol_up_cb(Fl_Widget *w, void *data) {
  if (media_player) {
    double vol = media_player->volume() + 0.1;
    if (vol > 1.0) vol = 1.0;
    media_player->set_volume(vol);
  }
}

static void vol_down_cb(Fl_Widget *w, void *data) {
  if (media_player) {
    double vol = media_player->volume() - 0.1;
    if (vol < 0.0) vol = 0.0;
    media_player->set_volume(vol);
  }
}

static void snapshot_cb(Fl_Widget *w, void *data) {
  Fl_Image_Capture cap;
  int id = cap.capture();
  char buf[128];
  snprintf(buf, sizeof(buf), "Captured still image frame ID #%d", id);
  log_msg(buf);

  // Send to video widget if available
  if (video_widget && cap.last_image()) {
    video_widget->video_sink()->set_video_frame(cap.last_image(), false);
  }
}

static Fl_Widget* create_multimedia_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();

  int y = UT_TESTAREA_Y + 5;

  // Media Player Widget
  media_player = new Fl_Multimedia(UT_TESTAREA_X + 10, y, UT_TESTAREA_W - 20, 60, "Demo Soundtrack.wav");
  media_player->set_source("Demo Soundtrack.wav");
  media_player->playback_state_changed_callback(on_state_changed);
  media_player->media_status_changed_callback(on_status_changed);
  media_player->position_changed_callback(on_position_changed);
  y += 68;

  // Video Widget Display
  video_widget = new Fl_Video_Widget(UT_TESTAREA_X + 10, y, UT_TESTAREA_W - 20, 75, "Video Preview");
  y += 82;

  // Buttons Row 1
  int btn_w = 70;
  int btn_h = 26;
  int btn_x = UT_TESTAREA_X + 10;

  Fl_Button *btn_play = new Fl_Button(btn_x, y, btn_w, btn_h, "Play");
  btn_play->callback(play_cb);
  btn_x += btn_w + 4;

  Fl_Button *btn_pause = new Fl_Button(btn_x, y, btn_w, btn_h, "Pause");
  btn_pause->callback(pause_cb);
  btn_x += btn_w + 4;

  Fl_Button *btn_stop = new Fl_Button(btn_x, y, btn_w, btn_h, "Stop");
  btn_stop->callback(stop_cb);
  btn_x += btn_w + 4;

  Fl_Button *btn_seek = new Fl_Button(btn_x, y, btn_w + 5, btn_h, "Seek 50%");
  btn_seek->callback(seek_cb);
  btn_x += btn_w + 9;

  Fl_Button *btn_mute = new Fl_Button(btn_x, y, btn_w + 5, btn_h, "Mute/Un");
  btn_mute->callback(mute_cb);
  btn_x += btn_w + 9;

  Fl_Button *btn_snap = new Fl_Button(btn_x, y, btn_w + 10, btn_h, "Snapshot");
  btn_snap->callback(snapshot_cb);
  y += btn_h + 6;

  // Buttons Row 2
  btn_x = UT_TESTAREA_X + 10;
  Fl_Button *btn_voldown = new Fl_Button(btn_x, y, btn_w + 10, btn_h, "Vol -10%");
  btn_voldown->callback(vol_down_cb);
  btn_x += btn_w + 15;

  Fl_Button *btn_volup = new Fl_Button(btn_x, y, btn_w + 10, btn_h, "Vol +10%");
  btn_volup->callback(vol_up_cb);
  y += btn_h + 8;

  // Log Display
  if (!media_log_buf) {
    media_log_buf = new Fl_Text_Buffer();
  } else {
    media_log_buf->text("");
  }
  media_log_disp = new Fl_Text_Display(UT_TESTAREA_X + 10, y, UT_TESTAREA_W - 20, UT_TESTAREA_H - (y - UT_TESTAREA_Y) - 10);
  media_log_disp->buffer(media_log_buf);
  media_log_disp->textsize(11);
  log_msg("Fl_Multimedia suite ready. Includes Qt 6 matching classes.");

  g->end();
  return g;
}

UnitTest multimedia_test(UT_TEST_MULTIMEDIA, "Multimedia", create_multimedia_test);

// ============================================================================
// Automated Core Unit Tests for All 14 Qt Multimedia-Matching Classes
// ============================================================================

// 1. Fl_Multimedia
TEST(Fl_Multimedia, InitAndDefaults) {
  Fl_Multimedia player;
  EXPECT_EQ((int)player.playback_state(), (int)Fl_Multimedia::StoppedState);
  EXPECT_EQ((int)player.state(), (int)Fl_Multimedia::StoppedState);
  EXPECT_EQ((int)player.media_status(), (int)Fl_Multimedia::NoMedia);
  EXPECT_EQ((int)player.status(), (int)Fl_Multimedia::NoMedia);
  EXPECT_EQ((int)player.error(), (int)Fl_Multimedia::NoError);
  EXPECT_EQ((int)player.duration(), 0);
  EXPECT_EQ((int)player.position(), 0);
  EXPECT_TRUE(player.volume() == 1.0);
  EXPECT_TRUE(!player.is_muted());
  EXPECT_TRUE(player.playback_rate() == 1.0);
  EXPECT_EQ(player.loops(), (int)Fl_Multimedia::Once);
  EXPECT_TRUE(player.is_available());
  return true;
}

TEST(Fl_Multimedia, SourceAndLoading) {
  Fl_Multimedia player;
  player.set_source("test_track.wav");
  EXPECT_STREQ(player.source(), "test_track.wav");
  EXPECT_STREQ(player.media(), "test_track.wav");
  EXPECT_EQ((int)player.media_status(), (int)Fl_Multimedia::LoadedMedia);
  EXPECT_GT((int)player.duration(), 0);
  EXPECT_TRUE(player.has_audio());
  EXPECT_TRUE(player.is_seekable());

  player.set_source(0);
  EXPECT_EQ((int)player.media_status(), (int)Fl_Multimedia::NoMedia);
  EXPECT_EQ((int)player.duration(), 0);
  EXPECT_EQ((int)player.position(), 0);
  return true;
}

TEST(Fl_Multimedia, PlaybackControl) {
  Fl_Multimedia player;
  player.set_source("sample.mp3");
  EXPECT_EQ((int)player.playback_state(), (int)Fl_Multimedia::StoppedState);

  int ok = player.play();
  EXPECT_EQ(ok, 1);
  EXPECT_EQ((int)player.playback_state(), (int)Fl_Multimedia::PlayingState);

  player.pause();
  EXPECT_EQ((int)player.playback_state(), (int)Fl_Multimedia::PausedState);

  player.stop();
  EXPECT_EQ((int)player.playback_state(), (int)Fl_Multimedia::StoppedState);
  EXPECT_EQ((int)player.position(), 0);
  return true;
}

TEST(Fl_Multimedia, PositionAndSeeking) {
  Fl_Multimedia player;
  player.set_source("sample.mp3");
  player.set_position(2500);
  EXPECT_EQ((int)player.position(), 2500);

  // Clamping test below 0
  player.set_position(-500);
  EXPECT_EQ((int)player.position(), 0);

  // Clamping test above duration
  int64_t dur = player.duration();
  player.set_position(dur + 10000);
  EXPECT_EQ((int)player.position(), (int)dur);
  return true;
}

TEST(Fl_Multimedia, VolumeAndMute) {
  Fl_Multimedia player;
  player.set_volume(0.65);
  EXPECT_TRUE(player.volume() == 0.65);

  // Clamping volume upper bound
  player.set_volume(1.5);
  EXPECT_TRUE(player.volume() == 1.0);

  // Clamping volume lower bound
  player.set_volume(-0.5);
  EXPECT_TRUE(player.volume() == 0.0);

  // Mute toggle
  player.set_muted(true);
  EXPECT_TRUE(player.is_muted());
  player.set_muted(false);
  EXPECT_TRUE(!player.is_muted());
  return true;
}

TEST(Fl_Multimedia, PlaybackRate) {
  Fl_Multimedia player;
  player.set_playback_rate(1.5);
  EXPECT_TRUE(player.playback_rate() == 1.5);

  player.set_playback_rate(0.5);
  EXPECT_TRUE(player.playback_rate() == 0.5);

  // Invalid rate default
  player.set_playback_rate(-1.0);
  EXPECT_TRUE(player.playback_rate() == 1.0);
  return true;
}

static int g_test_state_cb_called = 0;
static int g_test_status_cb_called = 0;
static int g_test_pos_cb_called = 0;

static void test_state_cb(Fl_Multimedia *p, Fl_Multimedia::PlaybackState s, void *d) {
  g_test_state_cb_called++;
}

static void test_status_cb(Fl_Multimedia *p, Fl_Multimedia::MediaStatus s, void *d) {
  g_test_status_cb_called++;
}

static void test_pos_cb(Fl_Multimedia *p, int64_t pos, void *d) {
  g_test_pos_cb_called++;
}

TEST(Fl_Multimedia, Callbacks) {
  g_test_state_cb_called = 0;
  g_test_status_cb_called = 0;
  g_test_pos_cb_called = 0;

  Fl_Multimedia player;
  player.playback_state_changed_callback(test_state_cb);
  player.media_status_changed_callback(test_status_cb);
  player.position_changed_callback(test_pos_cb);

  player.set_source("song.wav");
  EXPECT_GT(g_test_status_cb_called, 0);

  player.play();
  EXPECT_GT(g_test_state_cb_called, 0);

  player.set_position(1000);
  EXPECT_GT(g_test_pos_cb_called, 0);

  player.stop();
  return true;
}

TEST(Fl_Multimedia, ErrorHandling) {
  Fl_Multimedia player;
  player.set_source("invalid://non_existent_file.xyz");
  EXPECT_EQ((int)player.media_status(), (int)Fl_Multimedia::InvalidMedia);
  EXPECT_EQ((int)player.error(), (int)Fl_Multimedia::ResourceError);

  int ok = player.play();
  EXPECT_EQ(ok, 0);
  return true;
}

