//
// Fl_Video_Sink class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Video_Sink_H
#define Fl_Video_Sink_H

#include "Fl_Export.H"
#include <FL/Fl_Image.H>

/**
  \class Fl_Video_Sink
  \brief Receives, stores, and notifies listeners of new video frames, matching Qt's QVideoSink.
*/
class FL_EXPORT Fl_Video_Sink {
public:
  typedef void (*Fl_Video_Frame_Cb)(Fl_Video_Sink *sink, Fl_RGB_Image *frame, void *data);

private:
  Fl_RGB_Image *current_frame_;
  bool owns_frame_;
  Fl_Video_Frame_Cb frame_cb_;
  void *cb_data_;
  int frame_width_;
  int frame_height_;

public:
  Fl_Video_Sink();
  virtual ~Fl_Video_Sink();

  /** Sets a new video frame */
  void set_video_frame(Fl_RGB_Image *frame, bool take_ownership = false);

  /** Sets a new video frame from raw RGB buffer */
  void set_video_frame(const unsigned char *data, int w, int h, int depth = 3);

  /** Returns the current video frame */
  Fl_RGB_Image* video_frame() const { return current_frame_; }

  /** Returns the video frame dimensions */
  void video_size(int &w, int &h) const { w = frame_width_; h = frame_height_; }

  /** Returns video frame width */
  int video_width() const { return frame_width_; }

  /** Returns video frame height */
  int video_height() const { return frame_height_; }

  /** Registers a callback triggered when a new frame is delivered */
  void video_frame_changed_callback(Fl_Video_Frame_Cb cb, void *data = 0);
};

#endif
