//
// Fl_Media_Capture_Session implementation for the Fast Light Tool Kit (FLTK).
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

Fl_Media_Capture_Session::Fl_Media_Capture_Session()
  : camera_(0), audio_input_(0), screen_capture_(0), window_capture_(0),
    image_capture_(0), recorder_(0), video_sink_(0), video_output_(0),
    audio_output_(0) {
}

Fl_Media_Capture_Session::~Fl_Media_Capture_Session() {
}

void Fl_Media_Capture_Session::set_camera(Fl_Camera *camera) {
  camera_ = camera;
}

void Fl_Media_Capture_Session::set_audio_input(Fl_Audio_Input *input) {
  audio_input_ = input;
}

void Fl_Media_Capture_Session::set_screen_capture(Fl_Screen_Capture *screen) {
  screen_capture_ = screen;
}

void Fl_Media_Capture_Session::set_window_capture(Fl_Window_Capture *window) {
  window_capture_ = window;
}

void Fl_Media_Capture_Session::set_image_capture(Fl_Image_Capture *image_capture) {
  image_capture_ = image_capture;
  if (image_capture_) {
    image_capture_->set_capture_session(this);
  }
}

void Fl_Media_Capture_Session::set_recorder(Fl_Media_Recorder *recorder) {
  recorder_ = recorder;
  if (recorder_) {
    recorder_->set_capture_session(this);
  }
}

void Fl_Media_Capture_Session::set_video_sink(Fl_Video_Sink *sink) {
  video_sink_ = sink;
}

void Fl_Media_Capture_Session::set_video_output(Fl_Video_Widget *widget) {
  video_output_ = widget;
  if (video_output_) {
    video_sink_ = video_output_->video_sink();
  }
}

void Fl_Media_Capture_Session::set_audio_output(Fl_Audio_Output *output) {
  audio_output_ = output;
}
