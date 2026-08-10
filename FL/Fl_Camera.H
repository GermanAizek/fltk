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
public:
  /** Camera state enumeration */
  enum State {
    UnloadedState, ///< Camera is not initialized or no device selected
    LoadedState,   ///< Camera is loaded and ready, but not streaming
    ActiveState    ///< Camera is actively streaming frames
  };

  /** Camera error enumeration */
  enum Error {
    NoError,
    CameraError,
    InvalidRequestError,
    NotSupportedFeatureError
  };

private:
  Fl_Camera_Driver *driver_;
  State state_;
  Error error_;
  const char* error_string_;

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
  int start();

  /**
    Stops capturing from the camera.
  */
  void stop();

  /**
    Returns whether the camera is currently capturing.
    \return true if active (ActiveState), false otherwise
  */
  bool is_active() const { return state_ == ActiveState; }

  /** Returns the current state of the camera. */
  State state() const { return state_; }

  /** Returns the last error code. */
  Error error() const { return error_; }

  /** Returns a human-readable string for the last error. */
  const char* error_string() const { return error_string_; }

  /** Sets the camera device name or path (e.g. "/dev/video0") */
  void set_device(const char *dev_name);

  /** Returns the currently selected device name or path. */
  const char* device() const;

  /**
    Internal method used by the driver to notify the widget of a new frame.
  */
  void on_frame();

  /** Internal method to update the error state from the driver. */
  void set_error(Error e, const char *err_str);

  /** Internal method to update the camera state from the driver. */
  void set_state(State s);
};

#endif
