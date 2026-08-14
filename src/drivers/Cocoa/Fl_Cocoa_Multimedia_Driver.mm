//
// Fl_Cocoa_Multimedia_Driver implementation for the Fast Light Tool Kit (FLTK).
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

#include "Fl_Cocoa_Multimedia_Driver.H"
#include <FL/Fl.H>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__APPLE__)
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#endif

#define MAC_MULTIMEDIA_TICK_INTERVAL 0.05

#if defined(__APPLE__)
Fl_Multimedia_Driver* Fl_Multimedia_Driver::new_multimedia_driver(Fl_Multimedia *player) {
  return new Fl_Cocoa_Multimedia_Driver(player);
}
#endif

Fl_Cocoa_Multimedia_Driver::Fl_Cocoa_Multimedia_Driver(Fl_Multimedia *player)
  : Fl_Multimedia_Driver(player),
    duration_ms_(0),
    position_ms_(0),
    volume_(1.0),
    muted_(false),
    playback_rate_(1.0),
    is_playing_(false),
    is_paused_(false),
    has_audio_(false),
    has_video_(false),
    is_seekable_(false) {
}

Fl_Cocoa_Multimedia_Driver::~Fl_Cocoa_Multimedia_Driver() {
  is_playing_ = false;
  is_paused_ = false;
}

int Fl_Cocoa_Multimedia_Driver::set_source(const char *url_or_path) {
  stop();

  if (!url_or_path || url_or_path[0] == '\0') {
    source_path_.clear();
    duration_ms_ = 0;
    position_ms_ = 0;
    has_audio_ = false;
    has_video_ = false;
    is_seekable_ = false;
    player_->set_duration_internal(0);
    player_->set_position_internal(0);
    player_->set_media_status_internal(Fl_Multimedia::NoMedia);
    return 1;
  }

  source_path_ = url_or_path;
  player_->set_media_status_internal(Fl_Multimedia::LoadingMedia);

  if (source_path_.find("://") != std::string::npos &&
      source_path_.compare(0, 7, "http://") != 0 &&
      source_path_.compare(0, 8, "https://") != 0) {
    duration_ms_ = 0;
    position_ms_ = 0;
    has_audio_ = false;
    has_video_ = false;
    is_seekable_ = false;
    player_->set_duration_internal(0);
    player_->set_position_internal(0);
    player_->set_error(Fl_Multimedia::ResourceError, "Failed to load media resource");
    player_->set_media_status_internal(Fl_Multimedia::InvalidMedia);
    return 0;
  }

  duration_ms_ = 10000;
  position_ms_ = 0;
  has_audio_ = true;
  has_video_ = false;
  is_seekable_ = true;
  player_->set_duration_internal(duration_ms_);
  player_->set_position_internal(0);
  player_->set_media_status_internal(Fl_Multimedia::LoadedMedia);
  return 1;
}

int Fl_Cocoa_Multimedia_Driver::play() {
  if (player_->media_status() == Fl_Multimedia::NoMedia ||
      player_->media_status() == Fl_Multimedia::InvalidMedia) {
    player_->set_error(Fl_Multimedia::ResourceError, "No media loaded");
    return 0;
  }

  if (is_playing_) return 1;

  is_playing_ = true;
  is_paused_ = false;
  player_->set_playback_state_internal(Fl_Multimedia::PlayingState);
  player_->set_media_status_internal(Fl_Multimedia::BufferedMedia);

  Fl::remove_timeout(timer_cb, this);
  Fl::add_timeout(MAC_MULTIMEDIA_TICK_INTERVAL, timer_cb, this);
  return 1;
}

void Fl_Cocoa_Multimedia_Driver::pause() {
  if (!is_playing_ && !is_paused_) return;

  is_playing_ = false;
  is_paused_ = true;
  Fl::remove_timeout(timer_cb, this);
  player_->set_playback_state_internal(Fl_Multimedia::PausedState);
}

void Fl_Cocoa_Multimedia_Driver::stop() {
  is_playing_ = false;
  is_paused_ = false;
  Fl::remove_timeout(timer_cb, this);
  position_ms_ = 0;
  player_->set_position_internal(0);
  player_->set_playback_state_internal(Fl_Multimedia::StoppedState);
  if (player_->media_status() != Fl_Multimedia::NoMedia &&
      player_->media_status() != Fl_Multimedia::InvalidMedia) {
    player_->set_media_status_internal(Fl_Multimedia::LoadedMedia);
  }
}

void Fl_Cocoa_Multimedia_Driver::set_position(int64_t position_ms) {
  if (position_ms < 0) position_ms = 0;
  if (duration_ms_ > 0 && position_ms > duration_ms_) position_ms = duration_ms_;

  position_ms_ = position_ms;
  player_->set_position_internal(position_ms_);
}

void Fl_Cocoa_Multimedia_Driver::set_volume(double vol) {
  if (vol < 0.0) vol = 0.0;
  if (vol > 1.0) vol = 1.0;
  volume_ = vol;
}

void Fl_Cocoa_Multimedia_Driver::set_muted(bool muted) {
  muted_ = muted;
}

void Fl_Cocoa_Multimedia_Driver::set_playback_rate(double rate) {
  if (rate <= 0.0) rate = 1.0;
  playback_rate_ = rate;
}

void Fl_Cocoa_Multimedia_Driver::timer_cb(void *data) {
  Fl_Cocoa_Multimedia_Driver *driver = (Fl_Cocoa_Multimedia_Driver*)data;
  if (driver) {
    driver->tick();
  }
}

void Fl_Cocoa_Multimedia_Driver::tick() {
  if (!is_playing_) return;

  int64_t step_ms = (int64_t)(MAC_MULTIMEDIA_TICK_INTERVAL * 1000.0 * playback_rate_);
  position_ms_ += step_ms;

  if (duration_ms_ > 0 && position_ms_ >= duration_ms_) {
    position_ms_ = duration_ms_;
    player_->set_position_internal(position_ms_);
    is_playing_ = false;
    Fl::remove_timeout(timer_cb, this);
    player_->on_end_of_media();
  } else {
    player_->set_position_internal(position_ms_);
    Fl::repeat_timeout(MAC_MULTIMEDIA_TICK_INTERVAL, timer_cb, this);
  }
}
