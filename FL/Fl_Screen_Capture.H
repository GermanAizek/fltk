//
// Fl_Screen_Capture class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Screen_Capture_H
#define Fl_Screen_Capture_H

#include "Fl_Export.H"
#include <FL/Fl_Image.H>

class Fl_Screen_Capture_Driver;

/**
  \class Fl_Screen_Capture
  \brief Captures display screen output into video frames, matching Qt's QScreenCapture.
*/
class FL_EXPORT Fl_Screen_Capture {
public:
  typedef void (*Fl_Screen_Frame_Cb)(Fl_Screen_Capture *cap, Fl_RGB_Image *frame, void *data);

private:
  Fl_Screen_Capture_Driver *driver_;
  int screen_num_;
  bool active_;
  Fl_RGB_Image *last_frame_;
  Fl_Screen_Frame_Cb frame_cb_;
  void *cb_data_;

public:
  Fl_Screen_Capture();
  virtual ~Fl_Screen_Capture();

  /** Sets the screen index to capture */
  void set_screen(int screen_num);

  /** Returns the current screen index */
  int screen() const { return screen_num_; }

  /** Starts capturing screen frames */
  int start();

  /** Stops screen capture */
  void stop();

  /** Returns whether screen capture is active */
  bool is_active() const { return active_; }

  /** Captures a single frame from the screen */
  Fl_RGB_Image* capture_frame();

  /** Registers a frame ready callback */
  void frame_changed_callback(Fl_Screen_Frame_Cb cb, void *data = 0);
};

#endif
