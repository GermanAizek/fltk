//
// Fl_Audio_Buffer_Input implementation for the Fast Light Tool Kit (FLTK).
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

Fl_Audio_Buffer_Input::Fl_Audio_Buffer_Input(Fl_Audio_Sink *sink)
  : sink_(sink), total_bytes_sent_(0), ready_(true) {
}

Fl_Audio_Buffer_Input::~Fl_Audio_Buffer_Input() {
}

void Fl_Audio_Buffer_Input::set_audio_sink(Fl_Audio_Sink *sink) {
  sink_ = sink;
}

int Fl_Audio_Buffer_Input::send_audio_buffer(const void *data, int num_bytes, int sample_rate, int channels, int bits) {
  if (!data || num_bytes <= 0) return 0;

  total_bytes_sent_ += num_bytes;
  if (sink_) {
    return sink_->write(data, num_bytes);
  }
  return num_bytes;
}

void Fl_Audio_Buffer_Input::clear() {
  total_bytes_sent_ = 0;
}
