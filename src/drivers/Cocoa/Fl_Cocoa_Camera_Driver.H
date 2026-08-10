//
// Fl_Camera widget for the Fast Light Tool Kit (FLTK).
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


#ifndef Fl_Cocoa_Camera_Driver_H
#define Fl_Cocoa_Camera_Driver_H

#include "../../Fl_Camera_Driver.H"
#include <FL/fl_attr.h>

class Fl_Cocoa_Camera_Driver_Private;

class Fl_Cocoa_Camera_Driver : public Fl_Camera_Driver {
  Fl_Cocoa_Camera_Driver_Private *p;
  int running_;
public:
  Fl_Cocoa_Camera_Driver(Fl_Camera *widget);
  virtual ~Fl_Cocoa_Camera_Driver();

  int start() FL_OVERRIDE;
  void stop() FL_OVERRIDE;
  Fl_RGB_Image* get_frame() FL_OVERRIDE;
  
  void process_frame(void* sampleBufferPtr);
};

#endif
