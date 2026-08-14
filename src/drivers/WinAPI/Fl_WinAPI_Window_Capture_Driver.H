//
// Fl_WinAPI_Window_Capture_Driver for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_WinAPI_Window_Capture_Driver_H
#define Fl_WinAPI_Window_Capture_Driver_H

#include <FL/fl_attr.h>
#include <FL/Fl_Export.H>
#include "../../Fl_Window_Capture_Driver.H"

class Fl_WinAPI_Window_Capture_Driver : public Fl_Window_Capture_Driver {
public:
  Fl_WinAPI_Window_Capture_Driver(Fl_Window_Capture *capture);
  virtual ~Fl_WinAPI_Window_Capture_Driver();

  virtual int start() FL_OVERRIDE;
  virtual void stop() FL_OVERRIDE;
  virtual Fl_RGB_Image* capture_frame(Fl_Window *window) FL_OVERRIDE;
};

#endif
