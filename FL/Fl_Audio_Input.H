//
// Fl_Audio_Input class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Audio_Input_H
#define Fl_Audio_Input_H

#include "Fl_Export.H"

class Fl_Audio_Input_Driver;

/**
  \class Fl_Audio_Input
  \brief Accesses and configures an audio input device (microphone), matching Qt's QAudioInput.
*/
class FL_EXPORT Fl_Audio_Input {
private:
  Fl_Audio_Input_Driver *driver_;
  char *device_;
  double volume_;
  bool muted_;
  bool active_;

public:
  Fl_Audio_Input(const char *device_name = 0);
  virtual ~Fl_Audio_Input();

  /** Sets the audio input device name */
  void set_device(const char *dev);

  /** Returns the current audio input device name */
  const char* device() const { return device_ ? device_ : "default"; }

  /** Sets the input volume (0.0 to 1.0) */
  void set_volume(double vol);

  /** Returns the input volume level */
  double volume() const { return volume_; }

  /** Sets whether audio input is muted */
  void set_muted(bool muted);

  /** Returns whether audio input is muted */
  bool is_muted() const { return muted_; }

  /** Starts capturing audio input */
  int start();

  /** Stops capturing audio input */
  void stop();

  /** Returns whether audio input is currently active */
  bool is_active() const { return active_; }
};

#endif
