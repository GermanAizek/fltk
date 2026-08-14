//
// Fl_Multimedia_Driver base class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Multimedia_Driver_H
#define Fl_Multimedia_Driver_H

#include <FL/Fl_Multimedia.H>
#include <stdint.h>

/**
  \class Fl_Multimedia_Driver
  \brief Base abstraction for platform-specific multimedia playback engines.
*/
class Fl_Multimedia_Driver {
protected:
  Fl_Multimedia *player_;

public:
  Fl_Multimedia_Driver(Fl_Multimedia *player) : player_(player) {}
  virtual ~Fl_Multimedia_Driver() {}

  /** Sets the media source URL or file path */
  virtual int set_source(const char *url_or_path) = 0;

  /** Starts or resumes media playback */
  virtual int play() = 0;

  /** Pauses media playback */
  virtual void pause() = 0;

  /** Stops media playback */
  virtual void stop() = 0;

  /** Seeks to a position in milliseconds */
  virtual void set_position(int64_t position_ms) = 0;

  /** Returns the current playback position in milliseconds */
  virtual int64_t position() const = 0;

  /** Returns the media duration in milliseconds */
  virtual int64_t duration() const = 0;

  /** Sets the volume level (0.0 to 1.0) */
  virtual void set_volume(double vol) = 0;

  /** Returns the volume level */
  virtual double volume() const = 0;

  /** Sets whether audio is muted */
  virtual void set_muted(bool muted) = 0;

  /** Returns whether audio is muted */
  virtual bool is_muted() const = 0;

  /** Sets the playback rate multiplier */
  virtual void set_playback_rate(double rate) = 0;

  /** Returns the playback rate multiplier */
  virtual double playback_rate() const = 0;

  /** Returns true if current media contains audio */
  virtual bool has_audio() const = 0;

  /** Returns true if current media contains video */
  virtual bool has_video() const = 0;

  /** Returns true if media can be seeked */
  virtual bool is_seekable() const = 0;

  /** Returns true if backend is available */
  virtual bool is_available() const = 0;

  /** Factory method to instantiate the platform-specific driver */
  static Fl_Multimedia_Driver* new_multimedia_driver(Fl_Multimedia *player);
};

#endif
