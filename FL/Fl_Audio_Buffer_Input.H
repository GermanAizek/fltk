//
// Fl_Audio_Buffer_Input class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Audio_Buffer_Input_H
#define Fl_Audio_Buffer_Input_H

#include "Fl_Export.H"
#include "Fl_Audio_Sink.H"

/**
  \class Fl_Audio_Buffer_Input
  \brief Feeds audio buffers into audio sinks or recorders, matching Qt's QAudioBufferInput.
*/
class FL_EXPORT Fl_Audio_Buffer_Input {
private:
  Fl_Audio_Sink *sink_;
  int total_bytes_sent_;
  bool ready_;

public:
  Fl_Audio_Buffer_Input(Fl_Audio_Sink *sink = 0);
  virtual ~Fl_Audio_Buffer_Input();

  /** Sets the target audio sink */
  void set_audio_sink(Fl_Audio_Sink *sink);

  /** Returns target audio sink */
  Fl_Audio_Sink* audio_sink() const { return sink_; }

  /** Returns whether the input is ready */
  bool is_ready() const { return ready_; }

  /** Sends an audio buffer to the sink */
  int send_audio_buffer(const void *data, int num_bytes, int sample_rate = 44100, int channels = 2, int bits = 16);

  /** Returns total bytes sent */
  int total_bytes_sent() const { return total_bytes_sent_; }

  /** Resets counter */
  void clear();
};

#endif
