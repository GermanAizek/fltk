//
// Fl_Camera widget for the Fast Light Tool Kit (FLTK).
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

/** \file
   Fl_Camera widget.
*/

#ifndef Fl_Camera_H
#define Fl_Camera_H

#include "Fl_Widget.H"

class Fl_Camera_Driver;

/**
  \class Fl_Camera
  \brief A widget to display a video stream from a USB camera.

  The Fl_Camera widget connects to a local camera and displays the frames.
  It uses a background thread (or platform-specific async mechanism) to capture frames
  and automatically redraws the widget when a new frame is available.
*/
class FL_EXPORT Fl_Camera : public Fl_Widget {
  Fl_Camera_Driver *driver_;
  int playing_;

protected:
  /** Draws the current camera frame */
  void draw() FL_OVERRIDE;

public:
  /** 
    Creates a new Fl_Camera widget.
    \param[in] X, Y, W, H the dimensions and position
    \param[in] L an optional label
  */
  Fl_Camera(int X, int Y, int W, int H, const char *L=0);

  /**
    Destroys the Fl_Camera widget and stops capture if active.
  */
  virtual ~Fl_Camera();

  /**
    Starts capturing from the camera.
    \return 1 on success, 0 on failure
  */
  int play();

  /**
    Stops capturing from the camera.
  */
  void stop();

  /**
    Returns whether the camera is currently capturing.
    \return 1 if playing, 0 otherwise
  */
  int is_playing() const { return playing_; }

  /**
    Internal method used by the driver to notify the widget of a new frame.
  */
  void on_frame();
};

#endif
