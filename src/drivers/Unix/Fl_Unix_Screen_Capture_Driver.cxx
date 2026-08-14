//
// Fl_Unix_Screen_Capture_Driver implementation for the Fast Light Tool Kit (FLTK).
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

#include "Fl_Unix_Screen_Capture_Driver.H"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <stdlib.h>
#include <string.h>

Fl_Screen_Capture_Driver* Fl_Screen_Capture_Driver::new_screen_capture_driver(Fl_Screen_Capture *capture) {
  return new Fl_Unix_Screen_Capture_Driver(capture);
}

Fl_Unix_Screen_Capture_Driver::Fl_Unix_Screen_Capture_Driver(Fl_Screen_Capture *capture)
  : Fl_Screen_Capture_Driver(capture) {
}

Fl_Unix_Screen_Capture_Driver::~Fl_Unix_Screen_Capture_Driver() {
}

int Fl_Unix_Screen_Capture_Driver::start() {
  return 1;
}

void Fl_Unix_Screen_Capture_Driver::stop() {
}

Fl_RGB_Image* Fl_Unix_Screen_Capture_Driver::capture_frame(int screen_num) {
  int sx = 0, sy = 0, sw = 640, sh = 480;
  if (Fl::first_window()) {
    Fl::screen_xywh(sx, sy, sw, sh, screen_num);
  }
  if (sw <= 0 || sh <= 0) {
    sw = 640;
    sh = 480;
  }

  size_t buf_size = (size_t)sw * sh * 3;
  unsigned char *buf = new unsigned char[buf_size];
  if (!buf) return 0;

  Fl_Window *win = Fl::first_window();
  uchar *pixels = 0;
  if (win && win->shown()) {
    pixels = fl_read_image(buf, sx, sy, sw, sh, 0);
  }
  if (!pixels) {
    for (size_t i = 0; i < buf_size; i += 3) {
      buf[i] = 40;
      buf[i + 1] = 60;
      buf[i + 2] = 80;
    }
  }

  Fl_RGB_Image *img = new Fl_RGB_Image(buf, sw, sh, 3);
  img->alloc_array = 1;
  return img;
}
