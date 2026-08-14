//
// Fl_Unix_Audio_Sink_Driver implementation for the Fast Light Tool Kit (FLTK).
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

#include "Fl_Unix_Audio_Sink_Driver.H"
#include <FL/Fl_Audio_Sink.H>

Fl_Audio_Sink_Driver* Fl_Audio_Sink_Driver::new_audio_sink_driver(Fl_Audio_Sink *sink) {
  return new Fl_Unix_Audio_Sink_Driver(sink);
}

Fl_Unix_Audio_Sink_Driver::Fl_Unix_Audio_Sink_Driver(Fl_Audio_Sink *sink)
  : Fl_Audio_Sink_Driver(sink) {
}

Fl_Unix_Audio_Sink_Driver::~Fl_Unix_Audio_Sink_Driver() {
}

void Fl_Unix_Audio_Sink_Driver::start() {
}

void Fl_Unix_Audio_Sink_Driver::stop() {
}

void Fl_Unix_Audio_Sink_Driver::suspend() {
}

void Fl_Unix_Audio_Sink_Driver::resume() {
}

void Fl_Unix_Audio_Sink_Driver::reset() {
}

int Fl_Unix_Audio_Sink_Driver::write(const void *data, int num_bytes) {
  return num_bytes;
}

int Fl_Unix_Audio_Sink_Driver::bytes_free() const {
  return sink_ ? sink_->buffer_size() : 16384;
}
