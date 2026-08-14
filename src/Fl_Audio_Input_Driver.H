//
// Fl_Audio_Input_Driver base class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Audio_Input_Driver_H
#define Fl_Audio_Input_Driver_H

#include <FL/Fl_Export.H>
#include <FL/Fl_Audio_Input.H>

/**
  \class Fl_Audio_Input_Driver
  \brief Platform driver interface for audio input devices.
*/
class Fl_Audio_Input_Driver {
protected:
  Fl_Audio_Input *input_;

public:
  Fl_Audio_Input_Driver(Fl_Audio_Input *input) : input_(input) {}
  virtual ~Fl_Audio_Input_Driver() {}

  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void set_device(const char *device_name) = 0;

  static Fl_Audio_Input_Driver* new_audio_input_driver(Fl_Audio_Input *input);
};

#endif
