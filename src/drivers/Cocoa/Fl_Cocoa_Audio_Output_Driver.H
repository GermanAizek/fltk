//
// Fl_Cocoa_Audio_Output_Driver for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Cocoa_Audio_Output_Driver_H
#define Fl_Cocoa_Audio_Output_Driver_H

#include <FL/fl_attr.h>
#include <FL/Fl_Export.H>
#include "../../Fl_Audio_Output_Driver.H"

class Fl_Cocoa_Audio_Output_Driver : public Fl_Audio_Output_Driver {
public:
  Fl_Cocoa_Audio_Output_Driver(Fl_Audio_Output *output);
  virtual ~Fl_Cocoa_Audio_Output_Driver();

  virtual void set_device(const char *device_name) FL_OVERRIDE;
  virtual void set_volume(double vol) FL_OVERRIDE;
  virtual void set_muted(bool muted) FL_OVERRIDE;
};

#endif
