//
// Fl_Camera widget implementation for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Camera.H>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Image.H>
#include "Fl_Camera_Driver.H"
#include <stdlib.h>
#include <string.h>

Fl_Camera_Driver::Fl_Camera_Driver(Fl_Camera *widget)
  : widget_(widget), frame_image_(0), width_(0), height_(0), device_name_(0) {
}

Fl_Camera_Driver::~Fl_Camera_Driver() {
  if (device_name_) free(device_name_);
}

void Fl_Camera_Driver::set_device(const char *name) {
  if (device_name_) free(device_name_);
  if (name) {
    device_name_ = strdup(name);
  } else {
    device_name_ = 0;
  }
}

const char* Fl_Camera_Driver::device() const {
  return device_name_;
}

Fl_Camera::Fl_Camera(int X, int Y, int W, int H, const char *L)
  : Fl_Widget(X, Y, W, H, L), driver_(0), state_(UnloadedState), error_(NoError), error_string_(0) {
  box(FL_FLAT_BOX);
  color(FL_BLACK);
  driver_ = Fl_Camera_Driver::new_camera_driver(this);
}

Fl_Camera::~Fl_Camera() {
  stop();
  if (driver_) delete driver_;
}

int Fl_Camera::start() {
  if (!driver_) {
    set_error(CameraError, "No driver available");
    return 0;
  }
  if (state_ == ActiveState) return 1;
  
  if (driver_->start()) {
    set_state(ActiveState);
    set_error(NoError, 0);
    return 1;
  } else {
    set_error(CameraError, "Failed to start camera");
    return 0;
  }
}

void Fl_Camera::stop() {
  if (!driver_) return;
  if (state_ != ActiveState) return;
  
  driver_->stop();
  set_state(LoadedState);
}

void Fl_Camera::set_device(const char *dev_name) {
  if (driver_) driver_->set_device(dev_name);
}

const char* Fl_Camera::device() const {
  if (driver_) return driver_->device();
  return 0;
}

void Fl_Camera::set_error(Error e, const char *err_str) {
  error_ = e;
  error_string_ = err_str;
}

void Fl_Camera::set_state(State s) {
  state_ = s;
}

void Fl_Camera::on_frame() {
  // Can be called from another thread, so we should use Fl::awake to schedule a redraw
  // Note: Fl::awake() without arguments will just wake up the main loop which will then
  // process damage and redraw. But we need to ensure redraw() is safe to call from here.
  // Actually, Fl_Widget::redraw() just sets the damage bits.
  redraw();
  Fl::awake();
}

void Fl_Camera::draw() {
  draw_box();
  if (driver_) {
    Fl_RGB_Image *img = driver_->get_frame();
    if (img) {
      int cx = x() + Fl::box_dx(box());
      int cy = y() + Fl::box_dy(box());
      int cw = w() - Fl::box_dw(box());
      int ch = h() - Fl::box_dh(box());
      
      fl_push_clip(cx, cy, cw, ch);
      
      // Center the image if it is smaller, or draw from top-left if larger
      int img_x = cx + (cw - img->w()) / 2;
      int img_y = cy + (ch - img->h()) / 2;
      if (img_x < cx) img_x = cx;
      if (img_y < cy) img_y = cy;

      img->draw(img_x, img_y, img->w(), img->h(), 0, 0);
      
      fl_pop_clip();
    } else {
      fl_color(FL_WHITE);
      fl_draw("No Camera Frame", x(), y(), w(), h(), FL_ALIGN_CENTER);
    }
  } else {
    fl_color(FL_WHITE);
    fl_draw("Camera Driver Unavailable", x(), y(), w(), h(), FL_ALIGN_CENTER);
  }
}
