//
// Fl_Cocoa_Audio_Output_Driver implementation for the Fast Light Tool Kit (FLTK).
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

#include "Fl_Cocoa_Audio_Output_Driver.H"

Fl_Audio_Output_Driver* Fl_Audio_Output_Driver::new_audio_output_driver(Fl_Audio_Output *output) {
  return new Fl_Cocoa_Audio_Output_Driver(output);
}

Fl_Cocoa_Audio_Output_Driver::Fl_Cocoa_Audio_Output_Driver(Fl_Audio_Output *output)
  : Fl_Audio_Output_Driver(output) {
}

Fl_Cocoa_Audio_Output_Driver::~Fl_Cocoa_Audio_Output_Driver() {
}

void Fl_Cocoa_Audio_Output_Driver::set_device(const char *device_name) {
}

void Fl_Cocoa_Audio_Output_Driver::set_volume(double vol) {
}

void Fl_Cocoa_Audio_Output_Driver::set_muted(bool muted) {
}
