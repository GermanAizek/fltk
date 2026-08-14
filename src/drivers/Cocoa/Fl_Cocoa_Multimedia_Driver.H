//
// Fl_Cocoa_Multimedia_Driver header for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Cocoa_Multimedia_Driver_H
#define Fl_Cocoa_Multimedia_Driver_H

#include "../../Fl_Multimedia_Driver.H"
#include <stdint.h>
#include <string>

class Fl_Cocoa_Multimedia_Driver : public Fl_Multimedia_Driver {
private:
  std::string source_path_;
  int64_t duration_ms_;
  int64_t position_ms_;
  double volume_;
  bool muted_;
  double playback_rate_;
  bool is_playing_;
  bool is_paused_;
  bool has_audio_;
  bool has_video_;
  bool is_seekable_;

  static void timer_cb(void *data);
  void tick();

public:
  Fl_Cocoa_Multimedia_Driver(Fl_Multimedia *player);
  virtual ~Fl_Cocoa_Multimedia_Driver();

  int set_source(const char *url_or_path) FL_OVERRIDE;
  int play() FL_OVERRIDE;
  void pause() FL_OVERRIDE;
  void stop() FL_OVERRIDE;
  void set_position(int64_t position_ms) FL_OVERRIDE;
  int64_t position() const FL_OVERRIDE { return position_ms_; }
  int64_t duration() const FL_OVERRIDE { return duration_ms_; }
  void set_volume(double vol) FL_OVERRIDE;
  double volume() const FL_OVERRIDE { return volume_; }
  void set_muted(bool muted) FL_OVERRIDE;
  bool is_muted() const FL_OVERRIDE { return muted_; }
  void set_playback_rate(double rate) FL_OVERRIDE;
  double playback_rate() const FL_OVERRIDE { return playback_rate_; }
  bool has_audio() const FL_OVERRIDE { return has_audio_; }
  bool has_video() const FL_OVERRIDE { return has_video_; }
  bool is_seekable() const FL_OVERRIDE { return is_seekable_; }
  bool is_available() const FL_OVERRIDE { return true; }
};

#endif
