//
// Fl_Video_Frame_Input implementation for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Video_Frame_Input.H>

Fl_Video_Frame_Input::Fl_Video_Frame_Input(Fl_Video_Sink *sink)
  : sink_(sink), ready_(true) {
}

Fl_Video_Frame_Input::~Fl_Video_Frame_Input() {
}

void Fl_Video_Frame_Input::set_video_sink(Fl_Video_Sink *sink) {
  sink_ = sink;
}

int Fl_Video_Frame_Input::send_video_frame(Fl_RGB_Image *frame, bool take_ownership) {
  if (!sink_ || !frame) return 0;
  sink_->set_video_frame(frame, take_ownership);
  return 1;
}

int Fl_Video_Frame_Input::send_video_frame(const unsigned char *data, int w, int h, int depth) {
  if (!sink_ || !data || w <= 0 || h <= 0) return 0;
  sink_->set_video_frame(data, w, h, depth);
  return 1;
}
