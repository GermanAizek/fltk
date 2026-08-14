//
// Fl_Window_Capture class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Window_Capture_H
#define Fl_Window_Capture_H

#include "Fl_Export.H"
#include <FL/Fl_Window.H>
#include <FL/Fl_Image.H>

class Fl_Window_Capture_Driver;

/**
  \class Fl_Window_Capture
  \brief Captures a top-level FLTK window into video frames, matching Qt's QWindowCapture.
*/
class FL_EXPORT Fl_Window_Capture {
public:
  typedef void (*Fl_Window_Frame_Cb)(Fl_Window_Capture *cap, Fl_RGB_Image *frame, void *data);

private:
  Fl_Window_Capture_Driver *driver_;
  Fl_Window *window_;
  bool active_;
  Fl_RGB_Image *last_frame_;
  Fl_Window_Frame_Cb frame_cb_;
  void *cb_data_;

public:
  Fl_Window_Capture(Fl_Window *window = 0);
  virtual ~Fl_Window_Capture();

  /** Sets the target window to capture */
  void set_window(Fl_Window *window);

  /** Returns target window */
  Fl_Window* window() const { return window_; }

  /** Starts window capture */
  int start();

  /** Stops window capture */
  void stop();

  /** Returns whether window capture is active */
  bool is_active() const { return active_; }

  /** Captures a single frame from the target window */
  Fl_RGB_Image* capture_frame();

  /** Registers a callback triggered when a new frame is captured */
  void frame_changed_callback(Fl_Window_Frame_Cb cb, void *data = 0);
};

#endif
