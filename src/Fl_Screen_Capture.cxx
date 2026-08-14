//
// Fl_Screen_Capture implementation for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Screen_Capture.H>
#include "Fl_Screen_Capture_Driver.H"
#include <stdlib.h>
#include <string.h>

Fl_Screen_Capture::Fl_Screen_Capture()
  : driver_(0), screen_num_(0), active_(false), last_frame_(0), frame_cb_(0), cb_data_(0) {
  driver_ = Fl_Screen_Capture_Driver::new_screen_capture_driver(this);
}

Fl_Screen_Capture::~Fl_Screen_Capture() {
  frame_cb_ = 0;
  active_ = false;
  if (driver_) {
    delete driver_;
    driver_ = 0;
  }
  if (last_frame_) {
    delete last_frame_;
    last_frame_ = 0;
  }
}

void Fl_Screen_Capture::set_screen(int screen_num) {
  screen_num_ = screen_num;
}

int Fl_Screen_Capture::start() {
  active_ = true;
  if (driver_) {
    driver_->start();
  }
  capture_frame();
  return 1;
}

void Fl_Screen_Capture::stop() {
  active_ = false;
  if (driver_) {
    driver_->stop();
  }
}

Fl_RGB_Image* Fl_Screen_Capture::capture_frame() {
  if (last_frame_) {
    delete last_frame_;
    last_frame_ = 0;
  }

  if (driver_) {
    last_frame_ = driver_->capture_frame(screen_num_);
  }

  if (frame_cb_ && last_frame_) {
    frame_cb_(this, last_frame_, cb_data_);
  }
  return last_frame_;
}

void Fl_Screen_Capture::frame_changed_callback(Fl_Screen_Frame_Cb cb, void *data) {
  frame_cb_ = cb;
  cb_data_ = data;
}
