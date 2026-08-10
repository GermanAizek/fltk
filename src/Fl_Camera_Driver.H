//
// Fl_Camera_Driver class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Camera_Driver_H
#define Fl_Camera_Driver_H

class Fl_Camera;
class Fl_RGB_Image;

/**
  \class Fl_Camera_Driver
  \brief Base abstraction for OS-specific camera drivers.
*/
class Fl_Camera_Driver {
protected:
  Fl_Camera *widget_;
  Fl_RGB_Image *frame_image_;
  int width_;
  int height_;

public:
  Fl_Camera_Driver(Fl_Camera *widget) : widget_(widget), frame_image_(0), width_(0), height_(0) {}
  virtual ~Fl_Camera_Driver() {}

  /**
    Starts capturing.
    \return 1 on success, 0 on error
  */
  virtual int start() = 0;

  /**
    Stops capturing.
  */
  virtual void stop() = 0;
  
  /**
    Gets the latest frame.
    \return A pointer to Fl_RGB_Image containing the latest frame.
  */
  virtual Fl_RGB_Image* get_frame() = 0;
  
  /** Factory function implemented by platform code */
  static Fl_Camera_Driver* new_camera_driver(Fl_Camera *widget);
};

#endif
