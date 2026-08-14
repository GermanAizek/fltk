//
// Fl_Media_Recorder implementation for the Fast Light Tool Kit (FLTK).
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
#include <FL/Fl_Media_Capture_Session.H>
#include <stdlib.h>
#include <string.h>

Fl_Media_Recorder::Fl_Media_Recorder()
  : session_(0), state_(StoppedState), error_(NoError), error_string_(0),
    output_location_(0), actual_location_(0), duration_ms_(0),
    state_cb_(0), state_data_(0), duration_cb_(0), duration_data_(0),
    error_cb_(0), error_data_(0) {
}

Fl_Media_Recorder::~Fl_Media_Recorder() {
  stop();
  if (output_location_) free(output_location_);
  if (actual_location_) free(actual_location_);
  if (error_string_) free(error_string_);
}

void Fl_Media_Recorder::set_capture_session(Fl_Media_Capture_Session *session) {
  session_ = session;
}

void Fl_Media_Recorder::set_output_location(const char *path) {
  if (output_location_) free(output_location_);
  output_location_ = path ? strdup(path) : 0;
}

void Fl_Media_Recorder::record() {
  if (state_ == RecordingState) return;

  state_ = RecordingState;
  if (!actual_location_ && output_location_) {
    actual_location_ = strdup(output_location_);
  }
  if (state_cb_) {
    state_cb_(this, state_, state_data_);
  }
}

void Fl_Media_Recorder::pause() {
  if (state_ != RecordingState) return;

  state_ = PausedState;
  if (state_cb_) {
    state_cb_(this, state_, state_data_);
  }
}

void Fl_Media_Recorder::stop() {
  if (state_ == StoppedState) return;

  state_ = StoppedState;
  if (state_cb_) {
    state_cb_(this, state_, state_data_);
  }
}

void Fl_Media_Recorder::recorder_state_changed_callback(Fl_Recorder_State_Cb cb, void *data) {
  state_cb_ = cb;
  state_data_ = data;
}

void Fl_Media_Recorder::duration_changed_callback(Fl_Recorder_Duration_Cb cb, void *data) {
  duration_cb_ = cb;
  duration_data_ = data;
}

void Fl_Media_Recorder::error_callback(Fl_Recorder_Error_Cb cb, void *data) {
  error_cb_ = cb;
  error_data_ = data;
}
