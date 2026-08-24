//
// X11 image reading routines for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2018 by Bill Spitzak and others.
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

#include <FL/Fl.H>
#include <FL/Fl_Image.H>
#include <FL/platform.H>
#include "Fl_Screen_Driver.H"

#include <algorithm>
#include <cstddef>
#include <memory>

/**
 Reads an RGB(A) image from the current window or off-screen buffer.
 \param[in] p     pixel buffer, or nullptr to allocate one
 \param[in] X,Y   position of top-left of image to read
 \param[in] W,H   width and height of image to read
 \param[in] alpha alpha value for image (0 for none)
 \returns pointer to pixel buffer that should be freed using `delete[]`,
          or `nullptr` if allocation failed.
 */
uchar *fl_read_image(uchar *p, const int X, const int Y, const int W, const int H, const int alpha) {
  uchar *image_data = nullptr;
  std::unique_ptr<Fl_RGB_Image> img;

  if (fl_find(fl_window) == nullptr) {
    img.reset(Fl::screen_driver()->read_win_rectangle(X, Y, W, H, nullptr));
    if (img != nullptr) {
      img->alloc_array = 1;
    }
  } else {
    img.reset(Fl_Screen_Driver::traverse_to_gl_subwindows(Fl_Window::current(), X, Y, W, H, nullptr));
  }

  if (img != nullptr) {
    const int depth = (alpha != 0) ? 4 : 3;

    // Convert pixel depth if necessary
    if (img->d() != depth) {
      const size_t total_bytes = static_cast<size_t>(img->w()) * static_cast<size_t>(img->h()) * static_cast<size_t>(depth);
      std::unique_ptr<unsigned char[]> data_buf(new unsigned char[total_bytes]);
      unsigned char *data = data_buf.get();

      if (depth == 4) {
        (void)std::fill_n(data, total_bytes, static_cast<unsigned char>(alpha));
      }

      unsigned char *d = data;
      const int ld = (img->ld() != 0) ? img->ld() : (img->w() * img->d());

      for (int r = 0; r < img->h(); ++r) {
        const unsigned char *q = img->array + (r * ld);
        for (int c = 0; c < img->w(); ++c) {
          d[0] = q[0];
          d[1] = q[1];
          d[2] = q[2];
          d += depth;
          q += img->d();
        }
      }

      // Transfer ownership of data_buf to new Fl_RGB_Image
      std::unique_ptr<Fl_RGB_Image> converted_img(new Fl_RGB_Image(data_buf.release(), img->w(), img->h(), depth));
      converted_img->alloc_array = 1;
      img = std::move(converted_img);
    }

    // Resize image if dimensions do not match
    if ((img->w() != W) || (img->h() != H)) {
      std::unique_ptr<Fl_Image> scaled_base(img->copy(W, H));
      auto *scaled_rgb = dynamic_cast<Fl_RGB_Image*>(scaled_base.get());
      if (scaled_rgb != nullptr) {
        (void)scaled_base.release();
        img.reset(scaled_rgb);
      }
    }

    if (img != nullptr) {
      const size_t output_bytes = static_cast<size_t>(W) * static_cast<size_t>(H) * static_cast<size_t>(depth);

      if (p != nullptr) {
        (void)std::copy_n(img->array, output_bytes, p);
        image_data = p;
      } else {
        std::unique_ptr<unsigned char[]> out_buf(new unsigned char[output_bytes]);
        (void)std::copy_n(img->array, output_bytes, out_buf.get());
        image_data = out_buf.release();
      }
    }
  }

  return image_data;
}

/** Captures the content of a rectangular zone of a mapped window.
 \param win a mapped Fl_Window (derived types including Fl_Gl_Window are also possible)
 \param x,y,w,h window area to be captured. Intersecting sub-windows are captured too.
 \return The captured pixels as an Fl_RGB_Image.
*/
Fl_RGB_Image *fl_capture_window(Fl_Window *win, const int x, const int y, const int w, const int h) {
  Fl_RGB_Image *rgb = nullptr;

  if ((win != nullptr) && (win->shown() != 0)) {
    rgb = Fl_Screen_Driver::traverse_to_gl_subwindows(win, x, y, w, h, nullptr);
    if (rgb != nullptr) {
      rgb->scale(w, h, 0, 1);
    }
  }

  return rgb;
}