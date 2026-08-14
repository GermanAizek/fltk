//
// Fl_Audio_Output class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Audio_Output_H
#define Fl_Audio_Output_H

#include "Fl_Export.H"

class Fl_Audio_Output_Driver;

/**
  \class Fl_Audio_Output
  \brief Accesses and configures an audio output device (speaker/headphones), matching Qt's QAudioOutput.
*/
class FL_EXPORT Fl_Audio_Output {
private:
  Fl_Audio_Output_Driver *driver_;
  char *device_;
  double volume_;
  bool muted_;

public:
  Fl_Audio_Output(const char *device_name = 0);
  virtual ~Fl_Audio_Output();

  /** Sets the audio output device name */
  void set_device(const char *dev);

  /** Returns the audio output device name */
  const char* device() const { return device_ ? device_ : "default"; }

  /** Sets the output volume (0.0 to 1.0) */
  void set_volume(double vol);

  /** Returns the output volume */
  double volume() const { return volume_; }

  /** Sets whether audio output is muted */
  void set_muted(bool muted);

  /** Returns whether audio output is muted */
  bool is_muted() const { return muted_; }
};

#endif
