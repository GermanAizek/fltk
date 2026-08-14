//
// Fl_Video_Sink implementation for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Video_Sink.H>
#include <stdlib.h>
#include <string.h>

Fl_Video_Sink::Fl_Video_Sink()
  : current_frame_(0), owns_frame_(false), frame_cb_(0), cb_data_(0),
    frame_width_(0), frame_height_(0) {
}

Fl_Video_Sink::~Fl_Video_Sink() {
  if (current_frame_ && owns_frame_) {
    delete current_frame_;
    current_frame_ = 0;
  }
}

void Fl_Video_Sink::set_video_frame(Fl_RGB_Image *frame, bool take_ownership) {
  if (current_frame_ && owns_frame_) {
    delete current_frame_;
  }
  current_frame_ = frame;
  owns_frame_ = take_ownership;

  if (current_frame_) {
    frame_width_ = current_frame_->w();
    frame_height_ = current_frame_->h();
  } else {
    frame_width_ = 0;
    frame_height_ = 0;
  }

  if (frame_cb_) {
    frame_cb_(this, current_frame_, cb_data_);
  }
}

void Fl_Video_Sink::set_video_frame(const unsigned char *data, int w, int h, int depth) {
  if (!data || w <= 0 || h <= 0 || depth <= 0) {
    set_video_frame(0, false);
    return;
  }

  size_t size = (size_t)w * h * depth;
  unsigned char *buf = new unsigned char[size];
  if (buf) {
    memcpy(buf, data, size);
    Fl_RGB_Image *img = new Fl_RGB_Image(buf, w, h, depth);
    img->alloc_array = 1; // Fl_RGB_Image deletes with delete[]
    set_video_frame(img, true);
  }
}

void Fl_Video_Sink::video_frame_changed_callback(Fl_Video_Frame_Cb cb, void *data) {
  frame_cb_ = cb;
  cb_data_ = data;
}
