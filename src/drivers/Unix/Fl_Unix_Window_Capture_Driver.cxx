//
// Fl_Unix_Window_Capture_Driver implementation for the Fast Light Tool Kit (FLTK).
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

#include "Fl_Unix_Window_Capture_Driver.H"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <stdlib.h>
#include <string.h>

Fl_Window_Capture_Driver* Fl_Window_Capture_Driver::new_window_capture_driver(Fl_Window_Capture *capture) {
  return new Fl_Unix_Window_Capture_Driver(capture);
}

Fl_Unix_Window_Capture_Driver::Fl_Unix_Window_Capture_Driver(Fl_Window_Capture *capture)
  : Fl_Window_Capture_Driver(capture) {
}

Fl_Unix_Window_Capture_Driver::~Fl_Unix_Window_Capture_Driver() {
}

int Fl_Unix_Window_Capture_Driver::start() {
  return 1;
}

void Fl_Unix_Window_Capture_Driver::stop() {
}

Fl_RGB_Image* Fl_Unix_Window_Capture_Driver::capture_frame(Fl_Window *window) {
  int w = window ? window->w() : 320;
  int h = window ? window->h() : 240;

  if (w <= 0) w = 320;
  if (h <= 0) h = 240;

  size_t buf_size = (size_t)w * h * 3;
  unsigned char *buf = new unsigned char[buf_size];
  if (!buf) return 0;

  uchar *pixels = 0;
  if (window && window->shown()) {
    window->make_current();
    pixels = fl_read_image(buf, 0, 0, w, h, 0);
  }

  if (!pixels) {
    for (size_t i = 0; i < buf_size; i += 3) {
      buf[i] = 220;
      buf[i + 1] = 220;
      buf[i + 2] = 220;
    }
  }

  Fl_RGB_Image *img = new Fl_RGB_Image(buf, w, h, 3);
  img->alloc_array = 1;
  return img;
}
