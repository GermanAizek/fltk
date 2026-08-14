//
// Fl_Audio_Sink_Driver base class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Audio_Sink_Driver_H
#define Fl_Audio_Sink_Driver_H

#include <FL/Fl_Export.H>
#include <FL/Fl_Audio_Sink.H>
#include <stdint.h>

/**
  \class Fl_Audio_Sink_Driver
  \brief Platform driver interface for raw PCM audio output.
*/
class Fl_Audio_Sink_Driver {
protected:
  Fl_Audio_Sink *sink_;

public:
  Fl_Audio_Sink_Driver(Fl_Audio_Sink *sink) : sink_(sink) {}
  virtual ~Fl_Audio_Sink_Driver() {}

  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void suspend() = 0;
  virtual void resume() = 0;
  virtual void reset() = 0;
  virtual int write(const void *data, int num_bytes) = 0;
  virtual int bytes_free() const = 0;

  static Fl_Audio_Sink_Driver* new_audio_sink_driver(Fl_Audio_Sink *sink);
};

#endif
