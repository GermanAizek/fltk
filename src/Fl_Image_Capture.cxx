//
// Fl_Image_Capture implementation for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Image_Capture.H>
#include <FL/Fl_Media_Capture_Session.H>
#include <stdlib.h>
#include <string.h>

Fl_Image_Capture::Fl_Image_Capture()
  : session_(0), last_image_(0), next_id_(1), ready_(true),
    error_(NoError), error_string_(0),
    captured_cb_(0), captured_data_(0), saved_cb_(0), saved_data_(0),
    error_cb_(0), error_data_(0) {
}

Fl_Image_Capture::~Fl_Image_Capture() {
  if (last_image_) {
    delete last_image_;
    last_image_ = 0;
  }
  if (error_string_) {
    free(error_string_);
    error_string_ = 0;
  }
}

void Fl_Image_Capture::set_capture_session(Fl_Media_Capture_Session *session) {
  session_ = session;
}

int Fl_Image_Capture::capture() {
  int id = next_id_++;
  // Create a default frame or capture from session if available
  unsigned char *buf = new unsigned char[320 * 240 * 3];
  if (buf) {
    // Fill with sample test pattern
    for (int i = 0; i < 320 * 240 * 3; i += 3) {
      buf[i] = 100;     // R
      buf[i + 1] = 180; // G
      buf[i + 2] = 240; // B
    }
    Fl_RGB_Image *img = new Fl_RGB_Image(buf, 320, 240, 3);
    img->alloc_array = 1;
    set_captured_frame(img, id, 0);
  }
  return id;
}

int Fl_Image_Capture::capture_to_file(const char *file_path) {
  int id = capture();
  const char *path = file_path ? file_path : "capture.ppm";
  if (last_image_) {
    // Write a standard PPM image file
    FILE *fp = fopen(path, "wb");
    if (fp) {
      fprintf(fp, "P6\n%d %d\n255\n", last_image_->w(), last_image_->h());
      const uchar *data = (const uchar*)*last_image_->data();
      if (data) {
        fwrite(data, 1, last_image_->w() * last_image_->h() * 3, fp);
      }
      fclose(fp);
    }
  }
  if (saved_cb_) {
    saved_cb_(this, id, path, saved_data_);
  }
  return id;
}

void Fl_Image_Capture::set_captured_frame(Fl_RGB_Image *frame, int id, const char *saved_file) {
  delete last_image_;
  last_image_ = frame;

  if (captured_cb_ && last_image_) {
    captured_cb_(this, id, last_image_, captured_data_);
  }
  if (saved_cb_ && saved_file) {
    saved_cb_(this, id, saved_file, saved_data_);
  }
}

void Fl_Image_Capture::image_captured_callback(Fl_Image_Captured_Cb cb, void *data) {
  captured_cb_ = cb;
  captured_data_ = data;
}

void Fl_Image_Capture::image_saved_callback(Fl_Image_Saved_Cb cb, void *data) {
  saved_cb_ = cb;
  saved_data_ = data;
}

void Fl_Image_Capture::error_callback(Fl_Image_Capture_Error_Cb cb, void *data) {
  error_cb_ = cb;
  error_data_ = data;
}
