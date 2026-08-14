//
// Fl_Window_Capture_Driver base class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Window_Capture_Driver_H
#define Fl_Window_Capture_Driver_H

#include <FL/Fl_Export.H>
#include <FL/Fl_Window_Capture.H>

/**
  \class Fl_Window_Capture_Driver
  \brief Platform driver interface for top-level window capture.
*/
class Fl_Window_Capture_Driver {
protected:
  Fl_Window_Capture *capture_;

public:
  Fl_Window_Capture_Driver(Fl_Window_Capture *capture) : capture_(capture) {}
  virtual ~Fl_Window_Capture_Driver() {}

  virtual int start() = 0;
  virtual void stop() = 0;
  virtual Fl_RGB_Image* capture_frame(Fl_Window *window) = 0;

  static Fl_Window_Capture_Driver* new_window_capture_driver(Fl_Window_Capture *capture);
};

#endif
