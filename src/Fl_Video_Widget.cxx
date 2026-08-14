//
// Fl_Video_Widget implementation for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Video_Widget.H>
#include <FL/fl_draw.H>

void Fl_Video_Widget::on_sink_frame(Fl_Video_Sink *sink, Fl_RGB_Image *frame, void *data) {
  Fl_Video_Widget *widget = (Fl_Video_Widget*)data;
  if (widget) {
    widget->redraw();
  }
}

Fl_Video_Widget::Fl_Video_Widget(int X, int Y, int W, int H, const char *L)
  : Fl_Widget(X, Y, W, H, L), aspect_ratio_mode_(KeepAspectRatio), is_full_screen_(false) {
  box(FL_FLAT_BOX);
  color(FL_BLACK);
  video_sink_.video_frame_changed_callback(on_sink_frame, this);
}

Fl_Video_Widget::~Fl_Video_Widget() {
}

void Fl_Video_Widget::set_aspect_ratio_mode(AspectRatioMode mode) {
  aspect_ratio_mode_ = mode;
  redraw();
}

void Fl_Video_Widget::set_full_screen(bool full) {
  is_full_screen_ = full;
  redraw();
}

void Fl_Video_Widget::draw() {
  draw_box();

  int bx = x() + Fl::box_dx(box());
  int by = y() + Fl::box_dy(box());
  int bw = w() - Fl::box_dw(box());
  int bh = h() - Fl::box_dh(box());

  if (bw <= 0 || bh <= 0) return;

  fl_color(color());
  fl_rectf(bx, by, bw, bh);

  Fl_RGB_Image *frame = video_sink_.video_frame();
  if (frame && frame->w() > 0 && frame->h() > 0) {
    int fw = frame->w();
    int fh = frame->h();

    if (aspect_ratio_mode_ == IgnoreAspectRatio) {
      Fl_Image *scaled = frame->copy(bw, bh);
      if (scaled) {
        scaled->draw(bx, by);
        delete scaled;
      }
    } else {
      // KeepAspectRatio: letterbox / pillarbox
      double scale_x = (double)bw / (double)fw;
      double scale_y = (double)bh / (double)fh;
      double scale = (scale_x < scale_y) ? scale_x : scale_y;

      int target_w = (int)(fw * scale);
      int target_h = (int)(fh * scale);
      int offset_x = bx + (bw - target_w) / 2;
      int offset_y = by + (bh - target_h) / 2;

      Fl_Image *scaled = frame->copy(target_w, target_h);
      if (scaled) {
        scaled->draw(offset_x, offset_y);
        delete scaled;
      }
    }
  } else {
    // Placeholder video frame text
    fl_font(FL_HELVETICA, 12);
    fl_color(fl_rgb_color(100, 100, 100));
    fl_draw(label() ? label() : "Video Display", bx + bw / 2 - 40, by + bh / 2);
  }
}
