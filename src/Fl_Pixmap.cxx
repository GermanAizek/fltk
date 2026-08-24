//
// Pixmap drawing code for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2026 by Bill Spitzak and others.
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
#include <FL/platform.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Pixmap.H>
#include "flstring.h"

#include <array>
#include <cstdlib>
#include <cstring>

namespace {

// Parses XPM header without vararg sscanf
inline void parse_xpm_header(const char* str, int& ncolors, int& chars_per_pixel, int* height) {
  char* end = nullptr;
  // Skip width
  std::strtol(str, &end, 10);
  // Read height if requested
  const long h_val = std::strtol(end, &end, 10);
  if (height != nullptr) {
    *height = static_cast<int>(h_val);
  }
  // Read ncolors
  ncolors = static_cast<int>(std::strtol(end, &end, 10));
  // Read chars_per_pixel
  chars_per_pixel = static_cast<int>(std::strtol(end, &end, 10));
}

// Writes an integer to buffer without using vararg functions
inline char* write_int(char* ptr, const char* end, int val) {
  if (ptr >= end) {
    return ptr;
  }
  if (val < 0) {
    *ptr++ = '-';
    val = -val;
  }
  std::array<char, 16> temp{};
  int idx = 0;
  do {
    temp[idx++] = static_cast<char>('0' + (val % 10));
    val /= 10;
  } while (val > 0 && idx < 16);

  while (idx > 0 && ptr < end) {
    *ptr++ = temp[--idx];
  }
  return ptr;
}

// Formats "W H ncolors chars_per_pixel" into buffer without snprintf
inline size_t format_header(char* buf, const size_t buf_size, const int w, const int h,
                            const int ncolors,
                            const int cpp) {
  char* ptr = buf;
  const char* const end = buf + buf_size - 1;

  ptr = write_int(ptr, end, w);
  if (ptr < end) {
    *ptr++ = ' ';
  }
  ptr = write_int(ptr, end, h);
  if (ptr < end) {
    *ptr++ = ' ';
  }
  ptr = write_int(ptr, end, ncolors);
  if (ptr < end) {
    *ptr++ = ' ';
  }
  ptr = write_int(ptr, end, cpp);

  *ptr = '\0';
  return static_cast<size_t>(ptr - buf);
}

// Formats hex color line into buffer without snprintf
inline size_t format_hex_color(char* buf, const char* symbol, int cpp, uchar r, uchar g, uchar b) {
  static constexpr std::array<char, 17> hex_digits = {"0123456789ABCDEF"};
  char* ptr = buf;

  for (int i = 0; i < cpp; ++i) {
    *ptr++ = symbol[i];
  }
  *ptr++ = ' ';
  *ptr++ = 'c';
  *ptr++ = ' ';
  *ptr++ = '#';

  const auto ur = static_cast<unsigned int>(r);
  const auto ug = static_cast<unsigned int>(g);
  const auto ub = static_cast<unsigned int>(b);

  *ptr++ = hex_digits[(ur >> 4U) & 0x0FU];
  *ptr++ = hex_digits[ur & 0x0FU];
  *ptr++ = hex_digits[(ug >> 4U) & 0x0FU];
  *ptr++ = hex_digits[ug & 0x0FU];
  *ptr++ = hex_digits[(ub >> 4U) & 0x0FU];
  *ptr++ = hex_digits[ub & 0x0FU];

  *ptr = '\0';
  return static_cast<size_t>(ptr - buf);
}

} // namespace

void Fl_Pixmap::measure() {
  // ignore empty or bad pixmap data:
  if (w() < 0 && data()) {
    int W = 0;
    int H = 0;
    fl_measure_pixmap(data(), W, H);
    w(W);
    h(H);
    cache_w_ = cache_h_ = 0;
  }
}

void Fl_Pixmap::draw(int XP, int YP, int WP, int HP, int cx, int cy) {
  fl_graphics_driver->draw_pixmap(this, XP, YP, WP, HP, cx, cy);
}

/**
  The destructor frees all memory and server resources that are used by
  the pixmap.
*/
Fl_Pixmap::~Fl_Pixmap() {
  Fl_Pixmap::uncache();
  delete_data();
}

void Fl_Pixmap::uncache() {
  if (id_) {
    Fl_Graphics_Driver::default_driver().uncache_pixmap(id_);
    id_ = 0;
  }

  if (mask_) {
    Fl_Graphics_Driver::default_driver().delete_bitmask(mask_);
    mask_ = 0;
  }
  Fl_Image::uncache();
}

void Fl_Pixmap::label(Fl_Widget* widget) {
  widget->image(this);
}

void Fl_Pixmap::label(Fl_Menu_Item* m) {
  m->label(FL_IMAGE_LABEL, reinterpret_cast<const char*>(this));
}

void Fl_Pixmap::copy_data() {
  if (alloc_data) {
    return;
  }

  int ncolors = 0;
  int chars_per_pixel = 0;
  parse_xpm_header(data()[0], ncolors, chars_per_pixel, nullptr);
  const int chars_per_line = chars_per_pixel * data_w() + 1;

  const int total_rows = (ncolors < 0) ? (data_h() + 2) : (data_h() + ncolors + 1);
  char** new_data = new char*[total_rows];

  const size_t info_len = std::strlen(data()[0]) + 1;
  new_data[0] = new char[info_len];
  std::memcpy(new_data[0], data()[0], info_len);

  char** new_row = nullptr;

  if (ncolors < 0) {
    ncolors = -ncolors;
    new_row = new_data + 1;
    *new_row = new char[ncolors * 4];
    std::memcpy(*new_row, data()[1], ncolors * 4);
    ncolors = 1;
    new_row++;
  } else {
    new_row = new_data + 1;
    for (int i = 0; i < ncolors; ++i, ++new_row) {
      const size_t color_len = std::strlen(data()[i + 1]) + 1;
      *new_row = new char[color_len];
      std::memcpy(*new_row, data()[i + 1], color_len);
    }
  }

  for (int i = 0; i < data_h(); ++i, ++new_row) {
    *new_row = new char[chars_per_line];
    std::memcpy(*new_row, data()[i + ncolors + 1], chars_per_line);
  }

  data(new_data, total_rows);
  alloc_data = 1;
}

Fl_Image *Fl_Pixmap::copy(const int W, const int H) const {
  if (!data()) {
    return new Fl_Pixmap(static_cast<char *const*>(nullptr));
  }

  if (W == data_w() && H == data_h()) {
    auto* new_image = new Fl_Pixmap(data());
    new_image->copy_data();
    return new_image;
  }
  if (W <= 0 || H <= 0) {
    return nullptr;
  }

  int ncolors = 0;
  int chars_per_pixel = 0;
  parse_xpm_header(data()[0], ncolors, chars_per_pixel, nullptr);
  const int chars_per_line = chars_per_pixel * W + 1;

  std::array<char, 255> new_info{};
  const size_t info_len = format_header(new_info.data(), new_info.size(), W, H, ncolors, chars_per_pixel) + 1;

  const int xmod = data_w() % W;
  const int ymod = data_h() % H;
  const int ystep = data_h() / H;

  const int total_rows = (ncolors < 0) ? (H + 2) : (H + ncolors + 1);
  const auto new_data = new char*[total_rows];
  new_data[0] = new char[info_len];
  std::memcpy(new_data[0], new_info.data(), info_len);

  char** new_row = nullptr;

  if (ncolors < 0) {
    ncolors = -ncolors;
    new_row = new_data + 1;
    *new_row = new char[ncolors * 4];
    std::memcpy(*new_row, data()[1], ncolors * 4);
    ncolors = 1;
    new_row++;
  } else {
    new_row = new_data + 1;
    for (int i = 0; i < ncolors; ++i, ++new_row) {
      const size_t color_len = std::strlen(data()[i + 1]) + 1;
      *new_row = new char[color_len];
      std::memcpy(*new_row, data()[i + 1], color_len);
    }
  }

  auto* x_offset = new int[W];
  for (int dx = 0, err = W, current_x = 0; dx < W; ++dx) {
    x_offset[dx] = current_x * chars_per_pixel;
    current_x += data_w() / W;
    err -= xmod;
    if (err <= 0) {
      err += W;
      current_x++;
    }
  }

  int sy = 0;
  int yerr = H;
  for (int dy = H; dy > 0; --dy, ++new_row) {
    *new_row = new char[chars_per_line];
    char* new_ptr = *new_row;

    const char *line_ptr = data()[sy + ncolors + 1];

    if (chars_per_pixel == 1) {
      for (int dx = 0; dx < W; ++dx) {
        *new_ptr++ = line_ptr[x_offset[dx]];
      }
    } else if (chars_per_pixel == 2) {
      for (int dx = 0; dx < W; ++dx) {
        const char *old_ptr = line_ptr + x_offset[dx];
        *new_ptr++ = old_ptr[0];
        *new_ptr++ = old_ptr[1];
      }
    } else {
      for (int dx = 0; dx < W; ++dx) {
        const char *old_ptr = line_ptr + x_offset[dx];
        for (int c = 0; c < chars_per_pixel; ++c) {
          *new_ptr++ = old_ptr[c];
        }
      }
    }

    *new_ptr = '\0';
    sy += ystep;
    yerr -= ymod;
    if (yerr <= 0) {
      yerr += H;
      sy++;
    }
  }

  auto* new_image = new Fl_Pixmap(new_data);
  new_image->alloc_data = 1;

  delete[] x_offset;
  return new_image;
}

void Fl_Pixmap::color_average(const Fl_Color c, float i) {
  uncache();
  copy_data();

  uchar r = 0;
  uchar g = 0;
  uchar b = 0;
  Fl::get_color(c, r, g, b);

  if (i < 0.0F) {
    i = 0.0F;
  } else if (i > 1.0F) {
    i = 1.0F;
  }

  const auto ia = static_cast<unsigned int>(256.0F * i);
  const unsigned int ir = static_cast<unsigned int>(r) * (256U - ia);
  const unsigned int ig = static_cast<unsigned int>(g) * (256U - ia);
  const unsigned int ib = static_cast<unsigned int>(b) * (256U - ia);

  int ncolors = 0;
  int chars_per_pixel = 0;
  parse_xpm_header(data()[0], ncolors, chars_per_pixel, nullptr);

  if (ncolors < 0) {
    ncolors = -ncolors;
    auto* cmap = reinterpret_cast<uchar*>(const_cast<char*>(data()[1]));
    for (int color = 0; color < ncolors; ++color, cmap += 4) {
      cmap[1] = static_cast<uchar>((ia * cmap[1] + ir) >> 8U);
      cmap[2] = static_cast<uchar>((ia * cmap[2] + ig) >> 8U);
      cmap[3] = static_cast<uchar>((ia * cmap[3] + ib) >> 8U);
    }
  } else {
    std::array<char, 255> line{};
    for (int color = 0; color < ncolors; ++color) {
      const char *p = data()[color + 1] + chars_per_pixel + 1;
      const char *previous_word = p;
      for (;;) {
        while (*p && fl_ascii_isspace(*p)) {
          p++;
        }
        const char what = *p++;
        while (*p && !fl_ascii_isspace(*p)) {
          p++;
        }
        while (*p && fl_ascii_isspace(*p)) {
          p++;
        }
        if (!*p) {
          p = previous_word;
          break;
        }
        if (what == 'c') {
          break;
        }
        previous_word = p;
        while (*p && !fl_ascii_isspace(*p)) {
          p++;
        }
      }

      if (fl_parse_color(p, r, g, b)) {
        r = static_cast<uchar>((ia * static_cast<unsigned int>(r) + ir) >> 8U);
        g = static_cast<uchar>((ia * static_cast<unsigned int>(g) + ig) >> 8U);
        b = static_cast<uchar>((ia * static_cast<unsigned int>(b) + ib) >> 8U);

        const size_t line_len = format_hex_color(line.data(), data()[color + 1], chars_per_pixel, r, g, b) + 1;

        const auto mod_data = const_cast<char**>(data());
        delete[] mod_data[color + 1];
        mod_data[color + 1] = new char[line_len];
        std::memcpy(mod_data[color + 1], line.data(), line_len);
      }
    }
  }
}

void Fl_Pixmap::delete_data() const {
  if (alloc_data) {
    const auto* const* mod_data = data();
    for (int i = 0; i < count(); ++i) {
      delete[] const_cast<char*>(mod_data[i]);
    }
    delete[] const_cast<char**>(mod_data);
  }
}

void Fl_Pixmap::set_data(const char * const * p) {
  if (p) {
    int height = 0;
    int ncolors = 0;
    int chars_per_pixel = 0;
    parse_xpm_header(p[0], ncolors, chars_per_pixel, &height);
    if (ncolors < 0) {
      data(p, height + 2);
    } else {
      data(p, height + ncolors + 1);
    }
  }
}

void Fl_Pixmap::desaturate() {
  uncache();
  copy_data();

  int ncolors = 0;
  int chars_per_pixel = 0;
  parse_xpm_header(data()[0], ncolors, chars_per_pixel, nullptr);

  if (ncolors < 0) {
    ncolors = -ncolors;
    auto* cmap = reinterpret_cast<uchar*>(const_cast<char*>(data()[1]));
    for (int i = 0; i < ncolors; ++i, cmap += 4) {
      const auto g = static_cast<uchar>((cmap[1] * 31 + cmap[2] * 61 + cmap[3] * 8) / 100);
      cmap[1] = cmap[2] = cmap[3] = g;
    }
  } else {
    std::array<char, 255> line{};
    uchar r = 0;
    uchar g = 0;
    uchar b = 0;

    for (int i = 0; i < ncolors; ++i) {
      const char *p = data()[i + 1] + chars_per_pixel + 1;
      const char *previous_word = p;
      for (;;) {
        while (*p && fl_ascii_isspace(*p)) {
          p++;
        }
        const char what = *p++;
        while (*p && !fl_ascii_isspace(*p)) {
          p++;
        }
        while (*p && fl_ascii_isspace(*p)) {
          p++;
        }
        if (!*p) {
          p = previous_word;
          break;
        }
        if (what == 'c') {
          break;
        }
        previous_word = p;
        while (*p && !fl_ascii_isspace(*p)) {
          p++;
        }
      }

      if (fl_parse_color(p, r, g, b)) {
        g = static_cast<uchar>((r * 31 + g * 61 + b * 8) / 100);

        const size_t line_len = format_hex_color(line.data(), data()[i + 1], chars_per_pixel, g, g, g) + 1;

        const auto mod_data = const_cast<char**>(data());
        delete[] mod_data[i + 1];
        mod_data[i + 1] = new char[line_len];
        std::memcpy(mod_data[i + 1], line.data(), line_len);
      }
    }
  }
}