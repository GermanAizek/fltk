//
// Fl_Audio_Sink class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Audio_Sink_H
#define Fl_Audio_Sink_H

#include "Fl_Export.H"
#include <stdint.h>

class Fl_Audio_Sink_Driver;

/**
  \class Fl_Audio_Sink
  \brief Sends raw PCM audio data to an audio output device, matching Qt's QAudioSink.
*/
class FL_EXPORT Fl_Audio_Sink {
public:
  enum State {
    ActiveState    = 0,
    SuspendedState = 1,
    StoppedState   = 2
  };

private:
  Fl_Audio_Sink_Driver *driver_;
  char *device_;
  int sample_rate_;
  int channels_;
  int bit_depth_;
  double volume_;
  int buffer_size_;
  int bytes_written_;
  State state_;

public:
  Fl_Audio_Sink(const char *device_name = 0, int sample_rate = 44100, int channels = 2, int bit_depth = 16);
  virtual ~Fl_Audio_Sink();

  /** Starts the audio sink */
  void start();

  /** Stops the audio sink */
  void stop();

  /** Suspends audio output */
  void suspend();

  /** Resumes audio output */
  void resume();

  /** Resets the audio sink and clears buffers */
  void reset();

  /** Writes PCM audio data */
  int write(const void *data, int num_bytes);

  /** Returns current state */
  State state() const { return state_; }

  /** Sets the output volume (0.0 to 1.0) */
  void set_volume(double vol);

  /** Returns output volume */
  double volume() const { return volume_; }

  /** Returns configured audio sample rate in Hz */
  int sample_rate() const { return sample_rate_; }

  /** Returns configured audio channel count */
  int channels() const { return channels_; }

  /** Returns configured bit depth */
  int bit_depth() const { return bit_depth_; }

  /** Sets buffer size */
  void set_buffer_size(int size);

  /** Returns buffer size */
  int buffer_size() const { return buffer_size_; }

  /** Returns number of bytes free in the buffer */
  int bytes_free() const;

  /** Returns total bytes written since start */
  int bytes_written() const { return bytes_written_; }
};

#endif
