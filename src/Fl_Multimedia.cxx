//
// Fl_Multimedia implementation for the Fast Light Tool Kit (FLTK).
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
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include "Fl_Multimedia_Driver.H"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Fl_Multimedia::init_defaults() {
  source_ = 0;
  playback_state_ = StoppedState;
  media_status_ = NoMedia;
  error_ = NoError;
  error_string_ = 0;
  duration_ms_ = 0;
  position_ms_ = 0;
  volume_ = 1.0;
  muted_ = false;
  playback_rate_ = 1.0;
  loops_ = Once;
  loops_remaining_ = Once;

  state_cb_ = 0;
  state_data_ = 0;
  status_cb_ = 0;
  status_data_ = 0;
  position_cb_ = 0;
  position_data_ = 0;
  duration_cb_ = 0;
  duration_data_ = 0;
  error_cb_ = 0;
  error_data_ = 0;

  driver_ = Fl_Multimedia_Driver::new_multimedia_driver(this);
}

Fl_Multimedia::Fl_Multimedia(int X, int Y, int W, int H, const char *L)
  : Fl_Widget(X, Y, W, H, L), driver_(0) {
  box(FL_DOWN_BOX);
  color(FL_BLACK);
  selection_color(FL_BLUE);
  init_defaults();
}

Fl_Multimedia::Fl_Multimedia()
  : Fl_Widget(0, 0, 0, 0, 0), driver_(0) {
  init_defaults();
}

Fl_Multimedia::~Fl_Multimedia() {
  state_cb_ = 0;
  status_cb_ = 0;
  position_cb_ = 0;
  duration_cb_ = 0;
  error_cb_ = 0;
  if (driver_) {
    delete driver_;
    driver_ = 0;
  }
  if (source_) {
    free(source_);
    source_ = 0;
  }
  if (error_string_) {
    free(error_string_);
    error_string_ = 0;
  }
}

void Fl_Multimedia::set_source(const char *url_or_path) {
  if (source_) {
    free(source_);
    source_ = 0;
  }
  if (url_or_path) {
    source_ = strdup(url_or_path);
  }
  set_error(NoError, 0);
  loops_remaining_ = loops_;

  if (driver_) {
    driver_->set_source(url_or_path);
  }
  redraw();
}

int Fl_Multimedia::play() {
  if (!driver_) {
    set_error(ServiceMissingError, "No multimedia driver backend available");
    return 0;
  }
  if (media_status_ == NoMedia || media_status_ == InvalidMedia) {
    set_error(ResourceError, "Cannot play: no valid media loaded");
    return 0;
  }

  int res = driver_->play();
  redraw();
  return res;
}

void Fl_Multimedia::pause() {
  if (driver_) {
    driver_->pause();
  }
  redraw();
}

void Fl_Multimedia::stop() {
  if (driver_) {
    driver_->stop();
  }
  loops_remaining_ = loops_;
  redraw();
}

void Fl_Multimedia::set_position(int64_t position_ms) {
  if (driver_) {
    driver_->set_position(position_ms);
  }
  redraw();
}

bool Fl_Multimedia::is_seekable() const {
  if (driver_) return driver_->is_seekable();
  return false;
}

void Fl_Multimedia::set_volume(double vol) {
  if (vol < 0.0) vol = 0.0;
  if (vol > 1.0) vol = 1.0;
  volume_ = vol;
  if (driver_) {
    driver_->set_volume(volume_);
  }
  redraw();
}

void Fl_Multimedia::set_muted(bool muted) {
  muted_ = muted;
  if (driver_) {
    driver_->set_muted(muted_);
  }
  redraw();
}

void Fl_Multimedia::set_playback_rate(double rate) {
  if (rate <= 0.0) rate = 1.0;
  playback_rate_ = rate;
  if (driver_) {
    driver_->set_playback_rate(playback_rate_);
  }
}

void Fl_Multimedia::set_loops(int count) {
  loops_ = count;
  loops_remaining_ = count;
}

bool Fl_Multimedia::has_audio() const {
  if (driver_) return driver_->has_audio();
  return false;
}

bool Fl_Multimedia::has_video() const {
  if (driver_) return driver_->has_video();
  return false;
}

bool Fl_Multimedia::is_available() const {
  if (driver_) return driver_->is_available();
  return false;
}

void Fl_Multimedia::playback_state_changed_callback(Fl_Multimedia_State_Cb cb, void *data) {
  state_cb_ = cb;
  state_data_ = data;
}

void Fl_Multimedia::media_status_changed_callback(Fl_Multimedia_Status_Cb cb, void *data) {
  status_cb_ = cb;
  status_data_ = data;
}

void Fl_Multimedia::position_changed_callback(Fl_Multimedia_Position_Cb cb, void *data) {
  position_cb_ = cb;
  position_data_ = data;
}

void Fl_Multimedia::duration_changed_callback(Fl_Multimedia_Duration_Cb cb, void *data) {
  duration_cb_ = cb;
  duration_data_ = data;
}

void Fl_Multimedia::error_callback(Fl_Multimedia_Error_Cb cb, void *data) {
  error_cb_ = cb;
  error_data_ = data;
}

void Fl_Multimedia::set_playback_state_internal(PlaybackState state) {
  if (playback_state_ != state) {
    playback_state_ = state;
    if (state_cb_) {
      state_cb_(this, playback_state_, state_data_);
    }
    do_callback();
    redraw();
  }
}

void Fl_Multimedia::set_media_status_internal(MediaStatus status) {
  if (media_status_ != status) {
    media_status_ = status;
    if (status_cb_) {
      status_cb_(this, media_status_, status_data_);
    }
    redraw();
  }
}

void Fl_Multimedia::set_position_internal(int64_t position_ms) {
  if (position_ms_ != position_ms) {
    position_ms_ = position_ms;
    if (position_cb_) {
      position_cb_(this, position_ms_, position_data_);
    }
    redraw();
  }
}

void Fl_Multimedia::set_duration_internal(int64_t duration_ms) {
  if (duration_ms_ != duration_ms) {
    duration_ms_ = duration_ms;
    if (duration_cb_) {
      duration_cb_(this, duration_ms_, duration_data_);
    }
    redraw();
  }
}

void Fl_Multimedia::set_error(Error err, const char *msg) {
  error_ = err;
  if (error_string_) {
    free(error_string_);
    error_string_ = 0;
  }
  if (msg) {
    error_string_ = strdup(msg);
  }
  if (err != NoError && error_cb_) {
    error_cb_(this, error_, error_string_, error_data_);
  }
  redraw();
}

void Fl_Multimedia::on_end_of_media() {
  set_media_status_internal(EndOfMedia);

  if (loops_ == Infinite || loops_remaining_ > 1) {
    if (loops_remaining_ > 1) {
      loops_remaining_--;
    }
    set_position(0);
    play();
  } else {
    set_playback_state_internal(StoppedState);
  }
}

void Fl_Multimedia::draw() {
  if (w() <= 0 || h() <= 0) return;

  draw_box();

  int bx = x() + Fl::box_dx(box());
  int by = y() + Fl::box_dy(box());
  int bw = w() - Fl::box_dw(box());
  int bh = h() - Fl::box_dh(box());

  if (bw <= 10 || bh <= 10) return;

  fl_color(FL_BLACK);
  fl_rectf(bx, by, bw, bh);

  // Status & Title banner
  const char *state_text = "[STOPPED]";
  Fl_Color state_col = FL_GRAY;
  if (playback_state_ == PlayingState) {
    state_text = "[PLAYING]";
    state_col = fl_rgb_color(0, 200, 80);
  } else if (playback_state_ == PausedState) {
    state_text = "[PAUSED]";
    state_col = fl_rgb_color(255, 180, 0);
  }

  fl_font(FL_HELVETICA_BOLD, 12);
  fl_color(state_col);
  fl_draw(state_text, bx + 10, by + 18);

  // Source name / label
  const char *title = source_ ? source_ : (label() ? label() : "No Media Source");
  fl_font(FL_HELVETICA, 12);
  fl_color(FL_WHITE);
  fl_draw(title, bx + 95, by + 18);

  // Volume indicator
  char vol_str[32];
  if (muted_) {
    snprintf(vol_str, sizeof(vol_str), "Muted");
    fl_color(FL_RED);
  } else {
    snprintf(vol_str, sizeof(vol_str), "Vol: %d%%", (int)(volume_ * 100 + 0.5));
    fl_color(fl_rgb_color(160, 200, 255));
  }
  fl_draw(vol_str, bx + bw - 70, by + 18);

  // Timeline / Progress Bar
  int bar_y = by + bh - 24;
  int bar_h = 8;
  int bar_x = bx + 10;
  int bar_w = bw - 20;

  if (bar_w > 20 && bar_y > by + 20) {
    // Bar background
    fl_color(fl_rgb_color(50, 50, 50));
    fl_rectf(bar_x, bar_y, bar_w, bar_h);

    // Bar progress fill
    if (duration_ms_ > 0 && position_ms_ > 0) {
      double frac = (double)position_ms_ / (double)duration_ms_;
      if (frac > 1.0) frac = 1.0;
      int fill_w = (int)(bar_w * frac);
      fl_color(fl_rgb_color(0, 150, 255));
      fl_rectf(bar_x, bar_y, fill_w, bar_h);

      // Playhead circle
      fl_color(FL_WHITE);
      fl_circle(bar_x + fill_w, bar_y + bar_h / 2, 5);
    }

    // Time text mm:ss / mm:ss
    int cur_sec = (int)(position_ms_ / 1000);
    int tot_sec = (int)(duration_ms_ / 1000);
    char time_str[64];
    snprintf(time_str, sizeof(time_str), "%02d:%02d / %02d:%02d",
             cur_sec / 60, cur_sec % 60, tot_sec / 60, tot_sec % 60);

    fl_font(FL_HELVETICA, 10);
    fl_color(fl_rgb_color(180, 180, 180));
    fl_draw(time_str, bar_x, bar_y - 4);
  }
}

int Fl_Multimedia::handle(int event) {
  int bx = x() + Fl::box_dx(box());
  int by = y() + Fl::box_dy(box());
  int bw = w() - Fl::box_dw(box());
  int bh = h() - Fl::box_dh(box());

  int bar_y = by + bh - 24;
  int bar_h = 12;
  int bar_x = bx + 10;
  int bar_w = bw - 20;

  switch (event) {
    case FL_PUSH:
    case FL_DRAG: {
      int mx = Fl::event_x();
      int my = Fl::event_y();

      // Check click on timeline bar
      if (duration_ms_ > 0 && my >= bar_y - 6 && my <= bar_y + bar_h + 6 && mx >= bar_x && mx <= bar_x + bar_w) {
        double frac = (double)(mx - bar_x) / (double)bar_w;
        if (frac < 0.0) frac = 0.0;
        if (frac > 1.0) frac = 1.0;
        set_position((int64_t)(frac * duration_ms_));
        return 1;
      }

      // Check click on top area -> Toggle Play / Pause
      if (event == FL_PUSH && my < bar_y - 6) {
        if (playback_state_ == PlayingState) {
          pause();
        } else {
          play();
        }
        return 1;
      }
      return 1;
    }
    case FL_RELEASE:
      return 1;
    default:
      return Fl_Widget::handle(event);
  }
}
