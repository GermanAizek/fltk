//
// Fl_Media_Player implementation for the Fast Light Tool Kit (FLTK).
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

Fl_Media_Player::Fl_Media_Player()
  : audio_output_(0), video_sink_(0), video_output_(0) {
}

Fl_Media_Player::~Fl_Media_Player() {
  stop();
}

void Fl_Media_Player::set_source(const char *url_or_path) {
  player_engine_.set_source(url_or_path);
}

void Fl_Media_Player::set_audio_output(Fl_Audio_Output *output) {
  audio_output_ = output;
}

void Fl_Media_Player::set_video_sink(Fl_Video_Sink *sink) {
  video_sink_ = sink;
}

void Fl_Media_Player::set_video_output(Fl_Video_Widget *widget) {
  video_output_ = widget;
  if (video_output_) {
    video_sink_ = video_output_->video_sink();
  }
}
