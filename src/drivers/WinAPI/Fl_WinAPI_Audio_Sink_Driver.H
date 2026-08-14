//
// Fl_WinAPI_Audio_Sink_Driver for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_WinAPI_Audio_Sink_Driver_H
#define Fl_WinAPI_Audio_Sink_Driver_H

#include <FL/fl_attr.h>
#include <FL/Fl_Export.H>
#include "../../Fl_Audio_Sink_Driver.H"

class Fl_WinAPI_Audio_Sink_Driver : public Fl_Audio_Sink_Driver {
public:
  Fl_WinAPI_Audio_Sink_Driver(Fl_Audio_Sink *sink);
  virtual ~Fl_WinAPI_Audio_Sink_Driver();

  virtual void start() FL_OVERRIDE;
  virtual void stop() FL_OVERRIDE;
  virtual void suspend() FL_OVERRIDE;
  virtual void resume() FL_OVERRIDE;
  virtual void reset() FL_OVERRIDE;
  virtual int write(const void *data, int num_bytes) FL_OVERRIDE;
  virtual int bytes_free() const FL_OVERRIDE;
};

#endif
