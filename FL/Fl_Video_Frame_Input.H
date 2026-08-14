//
// Fl_Video_Frame_Input class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Video_Frame_Input_H
#define Fl_Video_Frame_Input_H

#include "Fl_Export.H"
#include "Fl_Video_Sink.H"

/**
  \class Fl_Video_Frame_Input
  \brief Feeds video frames from memory buffers into sinks or capture sessions, matching Qt's QVideoFrameInput.
*/
class FL_EXPORT Fl_Video_Frame_Input {
private:
  Fl_Video_Sink *sink_;
  bool ready_;

public:
  Fl_Video_Frame_Input(Fl_Video_Sink *sink = 0);
  virtual ~Fl_Video_Frame_Input();

  /** Sets the target video sink */
  void set_video_sink(Fl_Video_Sink *sink);

  /** Returns target video sink */
  Fl_Video_Sink* video_sink() const { return sink_; }

  /** Returns whether the input is ready to accept frames */
  bool is_ready() const { return ready_; }

  /** Sends an Fl_RGB_Image frame to the sink */
  int send_video_frame(Fl_RGB_Image *frame, bool take_ownership = false);

  /** Sends raw RGB memory buffer to the sink */
  int send_video_frame(const unsigned char *data, int w, int h, int depth = 3);
};

#endif
