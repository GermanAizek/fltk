//
// Fl_Unix_Audio_Input_Driver implementation for the Fast Light Tool Kit (FLTK).
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

#include "Fl_Unix_Audio_Input_Driver.H"

Fl_Audio_Input_Driver* Fl_Audio_Input_Driver::new_audio_input_driver(Fl_Audio_Input *input) {
  return new Fl_Unix_Audio_Input_Driver(input);
}

Fl_Unix_Audio_Input_Driver::Fl_Unix_Audio_Input_Driver(Fl_Audio_Input *input)
  : Fl_Audio_Input_Driver(input) {
}

Fl_Unix_Audio_Input_Driver::~Fl_Unix_Audio_Input_Driver() {
}

void Fl_Unix_Audio_Input_Driver::start() {
}

void Fl_Unix_Audio_Input_Driver::stop() {
}

void Fl_Unix_Audio_Input_Driver::set_device(const char *device_name) {
}
