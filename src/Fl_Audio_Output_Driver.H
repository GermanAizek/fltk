//
// Fl_Audio_Output_Driver base class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Audio_Output_Driver_H
#define Fl_Audio_Output_Driver_H

#include <FL/Fl_Export.H>
#include <FL/Fl_Audio_Output.H>

/**
  \class Fl_Audio_Output_Driver
  \brief Platform driver interface for audio output devices.
*/
class Fl_Audio_Output_Driver {
protected:
  Fl_Audio_Output *output_;

public:
  Fl_Audio_Output_Driver(Fl_Audio_Output *output) : output_(output) {}
  virtual ~Fl_Audio_Output_Driver() {}

  virtual void set_device(const char *device_name) = 0;
  virtual void set_volume(double vol) = 0;
  virtual void set_muted(bool muted) = 0;

  static Fl_Audio_Output_Driver* new_audio_output_driver(Fl_Audio_Output *output);
};

#endif
