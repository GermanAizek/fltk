//
// OpenGL text drawing support routines for the Fast Light Tool Kit (FLTK).
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

#include <config.h>

#if HAVE_GL || defined(FL_DOXYGEN)

#include <FL/Fl.H>
#include <FL/gl.h>
#include <FL/gl_draw.H>
#include <FL/fl_draw.H>
#include <FL/math.h>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Image_Surface.H>
#include "Fl_Scalable_Graphics_Driver.H"
#include "Fl_Gl_Window_Driver.H"
#if HAVE_GL_GLU_H
#  include <FL/glu.h>
#endif
#include <FL/glut.H>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#ifndef GL_TEXTURE_RECTANGLE_ARB
#  define GL_TEXTURE_RECTANGLE_ARB 0x84F5
#endif

extern float gl_start_scale;

static Fl_Font_Descriptor *gl_fontsize = nullptr;
static bool has_texture_rectangle = false;

static void gl_draw_invert(const char* str, int n, int x, int y) {
  glRasterPos2i(x, -y);
  gl_draw(str, n);
}

#if !defined(FL_DOXYGEN)

class gl_texture_fifo {
  friend class Fl_Gl_Window_Driver;

public:
  struct TextureData {
    GLuint texName = 0U;
    std::string utf8{};
    Fl_Font_Descriptor *fdesc = nullptr;
    float scale = 1.0F;
    int str_len = 0;
  };

  std::vector<TextureData> fifo{};
  int size_ = 0;
  int current = -1;
  int last = -1;
  bool textures_generated = false;

  void display_texture(int rank) const;
  int compute_texture(const char* str, int n);
  int already_known(const char *str, int n) const;

  explicit gl_texture_fifo(int max_elements = 100);
  ~gl_texture_fifo();

  gl_texture_fifo(const gl_texture_fifo&) = delete;
  gl_texture_fifo& operator=(const gl_texture_fifo&) = delete;
  gl_texture_fifo(gl_texture_fifo&&) = delete;
  gl_texture_fifo& operator=(gl_texture_fifo&&) = delete;

  int size() const { return size_; }
};

gl_texture_fifo::gl_texture_fifo(int max_elements)
  : size_(max_elements),
    current(-1),
    last(-1),
    textures_generated(false) {
  fifo.resize(static_cast<size_t>(size_));
}

gl_texture_fifo::~gl_texture_fifo() {
  if (textures_generated) {
    for (int i = 0; i < size_; ++i) {
      if (fifo[static_cast<size_t>(i)].texName != 0U) {
        glDeleteTextures(1, &fifo[static_cast<size_t>(i)].texName);
      }
    }
  }
}

int gl_texture_fifo::already_known(const char *str, int n) const {
  int result = -1;
  for (int rank = 0; rank <= last; ++rank) {
    const auto idx = static_cast<size_t>(rank);
    if ((fifo[idx].str_len == n) &&
        (fifo[idx].fdesc == gl_fontsize) &&
        (fifo[idx].scale == Fl_Gl_Window_Driver::global()->gl_scale) &&
        (fifo[idx].utf8.compare(0, static_cast<size_t>(n), str, static_cast<size_t>(n)) == 0)) {
      result = rank;
      break;
    }
  }
  return result;
}

void gl_texture_fifo::display_texture(int rank) const {
  glPushAttrib(GL_TRANSFORM_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  const float winw = Fl_Gl_Window_Driver::global()->gl_scale * static_cast<float>(Fl_Window::current()->w());
  const float winh = Fl_Gl_Window_Driver::global()->gl_scale * static_cast<float>(Fl_Window::current()->h());

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_LIGHTING);

  std::array<GLfloat, 4> pos = {0.0F, 0.0F, 0.0F, 0.0F};
  glGetFloatv(GL_CURRENT_RASTER_POSITION, pos.data());
  if (gl_start_scale != 1.0F) {
    pos[0] /= gl_start_scale;
    pos[1] /= gl_start_scale;
  }

  const float R = 2.0F;
  glScalef(R / winw, R / winh, 1.0F);
  glTranslatef(-winw / R, -winh / R, 0.0F);
  glEnable(GL_TEXTURE_RECTANGLE_ARB);

  const auto u_rank = static_cast<size_t>(rank);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, fifo[u_rank].texName);

  GLint width = 0;
  GLint height = 0;
  glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_ARB, 0, GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_ARB, 0, GL_TEXTURE_HEIGHT, &height);

  glBegin(GL_QUADS);
  const float ox = pos[0];
  const float oy = pos[1] + static_cast<float>(height) - (Fl_Gl_Window_Driver::global()->gl_scale * static_cast<float>(fl_descent()));

  glTexCoord2f(0.0F, 0.0F);
  glVertex2f(ox, oy);
  glTexCoord2f(0.0F, static_cast<GLfloat>(height));
  glVertex2f(ox, oy - static_cast<float>(height));
  glTexCoord2f(static_cast<GLfloat>(width), static_cast<GLfloat>(height));
  glVertex2f(ox + static_cast<float>(width), oy - static_cast<float>(height));
  glTexCoord2f(static_cast<GLfloat>(width), 0.0F);
  glVertex2f(ox + static_cast<float>(width), oy);
  glEnd();

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPopAttrib();

#if HAVE_GL_GLU_H
  pos[0] += static_cast<float>(width);
  std::array<GLdouble, 16> modelmat = {};
  glGetDoublev(GL_MODELVIEW_MATRIX, modelmat.data());
  std::array<GLdouble, 16> projmat = {};
  glGetDoublev(GL_PROJECTION_MATRIX, projmat.data());
  GLdouble objX = 0.0;
  GLdouble objY = 0.0;
  GLdouble objZ = 0.0;
  std::array<GLint, 4> viewport = {};
  glGetIntegerv(GL_VIEWPORT, viewport.data());
  (void)gluUnProject(static_cast<GLdouble>(pos[0]), static_cast<GLdouble>(pos[1]), static_cast<GLdouble>(pos[2]),
                     modelmat.data(), projmat.data(), viewport.data(), &objX, &objY, &objZ);

  if (gl_start_scale != 1.0F) {
    objX *= static_cast<double>(gl_start_scale);
    objY *= static_cast<double>(gl_start_scale);
  }
  glRasterPos2d(objX, objY);
#endif
}

int gl_texture_fifo::compute_texture(const char* str, int n) {
  current = (current + 1) % size_;
  if (current > last) {
    last = current;
  }

  const auto cur_idx = static_cast<size_t>(current);
  fifo[cur_idx].utf8.assign(str, static_cast<size_t>(n));
  fifo[cur_idx].str_len = n;

  const Fl_Fontsize fs = fl_size();
  const float s = fl_graphics_driver->scale();
  fl_graphics_driver->Fl_Graphics_Driver::scale(1.0F);

  const auto scaled_fs = static_cast<int>(static_cast<float>(fs) * Fl_Gl_Window_Driver::global()->gl_scale);
  fl_font(fl_font(), scaled_fs);

  int w = static_cast<int>(std::ceil(fl_width(fifo[cur_idx].utf8.c_str(), n)));
  w = ((w + 3) / 4) * 4;
  const int h = fl_height();

  fl_graphics_driver->Fl_Graphics_Driver::scale(s);
  fl_font(fl_font(), fs);

  fifo[cur_idx].scale = Fl_Gl_Window_Driver::global()->gl_scale;
  fifo[cur_idx].fdesc = gl_fontsize;

  const std::unique_ptr<char[]> alpha_buf(Fl_Gl_Window_Driver::global()->alpha_mask_for_string(str, n, w, h, scaled_fs));

  GLint row_length = 0;
  GLint alignment = 0;
  glGetIntegerv(GL_UNPACK_ROW_LENGTH, &row_length);
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);

  glPushAttrib(GL_TEXTURE_BIT);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, fifo[cur_idx].texName);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, w);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_ALPHA8, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, alpha_buf.get());
  glPopAttrib();

  glPixelStorei(GL_UNPACK_ROW_LENGTH, row_length);
  glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);

  return current;
}

static std::unique_ptr<gl_texture_fifo> gl_fifo;

#endif // !defined(FL_DOXYGEN)

/** Returns the current font's height */
int gl_height() {
  return fl_height();
}

/** Returns the current font's descent */
int gl_descent() {
  return fl_descent();
}

/** Returns the width of the string in the current font */
double gl_width(const char* s) {
  return fl_width(s);
}

/** Returns the width of n characters of the string in the current font */
double gl_width(const char* s, int n) {
  return fl_width(s, n);
}

/** Returns the width of the character in the current font */
double gl_width(uchar c) {
  return fl_width(c);
}

/** Sets the current OpenGL font to the same font as calling fl_font(). */
void gl_font(int fontid, int size) {
  static bool once = true;
  if (once) {
    once = false;
    if (Fl::draw_GL_text_with_textures() != 0) {
      int gl_version_major = 0;
      const auto *ver_str = reinterpret_cast<const char*>(glGetString(GL_VERSION));
      if (ver_str != nullptr) {
        std::istringstream(ver_str) >> gl_version_major;
      }
      if (gl_version_major >= 3) {
        has_texture_rectangle = true;
      } else {
        const auto *extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
        if (extensions != nullptr) {
          const std::string ext_str(extensions);
          has_texture_rectangle = (ext_str.find("GL_EXT_texture_rectangle") != std::string::npos) ||
                                  (ext_str.find("GL_ARB_texture_rectangle") != std::string::npos);
        }
      }
      Fl::draw_GL_text_with_textures(has_texture_rectangle ? 1 : 0);
    }
  }

  fl_font(fontid, size);
  Fl_Font_Descriptor *fl_fontsize = fl_graphics_driver->font_descriptor();
  if (!has_texture_rectangle) {
    Fl_Gl_Window_Driver::global()->gl_bitmap_font(fl_fontsize);
  }
  gl_fontsize = fl_fontsize;
}

void gl_remove_displaylist_fonts() {
  fl_graphics_driver->font(0, 0);

  for (int j = 0; j < FL_FREE_FONT; ++j) {
    Fl_Font_Descriptor *prevDesc = nullptr;
    Fl_Font_Descriptor *nextDesc = nullptr;
    Fl_Font_Descriptor *&firstDesc = *Fl_Gl_Window_Driver::global()->fontnum_to_fontdescriptor(j);

    for (Fl_Font_Descriptor *desc = firstDesc; desc != nullptr; desc = nextDesc) {
      nextDesc = desc->next;
      if (desc->listbase != 0U) {
        if (desc == firstDesc) {
          firstDesc = desc->next;
        } else if (prevDesc != nullptr) {
          prevDesc->next = desc->next;
        }
        glDeleteLists(desc->listbase, static_cast<GLsizei>(Fl_Gl_Window_Driver::global()->genlistsize()));
        delete desc;
      } else {
        prevDesc = desc;
      }
    }
  }
}

void gl_draw(const char* str, int n) {
  if (n > 0) {
    if (has_texture_rectangle) {
      Fl_Gl_Window_Driver::global()->draw_string_with_texture(str, n);
    } else {
      Fl_Gl_Window_Driver::global()->draw_string_legacy(str, n);
    }
  }
}

void gl_draw(const char* str, int n, int x, int y) {
  glRasterPos2i(x, y);
  gl_draw(str, n);
}

void gl_draw(const char* str, int n, float x, float y) {
  glRasterPos2f(x, y);
  gl_draw(str, n);
}

void gl_draw(const char* str) {
  if (str != nullptr) {
    gl_draw(str, static_cast<int>(std::char_traits<char>::length(str)));
  }
}

void gl_draw(const char* str, int x, int y) {
  if (str != nullptr) {
    gl_draw(str, static_cast<int>(std::char_traits<char>::length(str)), x, y);
  }
}

void gl_draw(const char* str, float x, float y) {
  if (str != nullptr) {
    gl_draw(str, static_cast<int>(std::char_traits<char>::length(str)), x, y);
  }
}

void gl_draw(const char* str, int x, int y, int w, int h, Fl_Align align) {
  fl_draw(str, x, -y - h, w, h, align, gl_draw_invert, nullptr, 0);
}

void gl_measure(const char* str, int& x, int& y) {
  fl_measure(str, x, y, 0);
}

void gl_rect(int x, int y, int w, int h) {
  int rx = x;
  int ry = y;
  int rw = w;
  int rh = h;

  if (rw < 0) {
    rw = -rw;
    rx = rx - rw;
  }
  if (rh < 0) {
    rh = -rh;
    ry = ry - rh;
  }

  glBegin(GL_LINE_LOOP);
  const int r = rx + rw - 1;
  const int b = ry + rh - 1;
  glVertex2i(r, b);
  glVertex2i(r, ry);
  glVertex2i(rx, ry);
  glVertex2i(rx, b);
  glEnd();
}

void gl_rectf(int x, int y, int w, int h) {
  glRecti(x, y, x + w, y + h);
}

void gl_draw_image(const uchar* b, int x, int y, int w, int h, int d, int ld) {
  const int row_len = (ld != 0) ? ld : (w * d);
  GLint current_row_length = 0;
  glGetIntegerv(GL_UNPACK_ROW_LENGTH, &current_row_length);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, row_len / d);
  glRasterPos2i(x, y);
  glDrawPixels(w, h, (d < 4) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<const void*>(b));
  glPixelStorei(GL_UNPACK_ROW_LENGTH, current_row_length);
}

void gl_color(Fl_Color i) {
  if (Fl_Gl_Window_Driver::global()->overlay_color(i) == 0) {
    uchar red = 0U;
    uchar green = 0U;
    uchar blue = 0U;
    Fl::get_color(i, red, green, blue);
    glColor3ub(red, green, blue);
  }
}

int gl_texture_pile_height() {
  if (!gl_fifo) {
    gl_fifo.reset(new gl_texture_fifo());
  }
  return gl_fifo->size();
}

void gl_texture_reset() {
  if (gl_fifo) {
    gl_texture_pile_height(gl_texture_pile_height());
  }
}

void gl_texture_pile_height(int max_elements) {
  gl_fifo.reset(new gl_texture_fifo(max_elements));
}

void Fl_Gl_Window_Driver::draw_string_legacy(const char* str, int n) {
  draw_string_legacy_glut(str, n);
}

void Fl_Gl_Window_Driver::draw_string_with_texture(const char* str, int n) {
  GLint valid = 0;
  glGetIntegerv(GL_CURRENT_RASTER_POSITION_VALID, &valid);
  if (valid != 0) {
    const Fl_Gl_Window *gwin = Fl_Window::current()->as_gl_window();
    gl_scale = (gwin != nullptr) ? gwin->pixels_per_unit() : 1.0F;

    if (!gl_fifo) {
      gl_fifo.reset(new gl_texture_fifo());
    }

    if (!gl_fifo->textures_generated) {
      if (has_texture_rectangle) {
        for (int i = 0; i < gl_fifo->size_; ++i) {
          glGenTextures(1, &(gl_fifo->fifo[static_cast<size_t>(i)].texName));
        }
      }
      gl_fifo->textures_generated = true;
    }

    int index = gl_fifo->already_known(str, n);
    if (index == -1) {
      index = gl_fifo->compute_texture(str, n);
    }
    gl_fifo->display_texture(index);
  }
}

char *Fl_Gl_Window_Driver::alpha_mask_for_string(const char *str, int n, int w, int h, Fl_Fontsize fs) {
  auto *image_surface = new Fl_Image_Surface(w, h);
  const Fl_Font fnt = fl_font();

  Fl_Surface_Device::push_current(image_surface);
  fl_color(0, 0, 0);
  fl_rectf(0, 0, w, h);
  fl_color(255, 255, 255);
  fl_font(fnt, fs);

  const int desc = fl_descent();
  fl_draw(str, n, 0, h - desc);

  const Fl_RGB_Image *image = image_surface->image();
  Fl_Surface_Device::pop_current();
  delete image_surface;

  const auto total_pixels = static_cast<size_t>(w * h);
  auto *alpha_buf = new char[total_pixels];
  if (image != nullptr) {
    for (size_t idx = 0; idx < total_pixels; ++idx) {
      alpha_buf[idx] = static_cast<char>(image->array[idx * 3U + 1U]);
    }
  }
  delete image;
  return alpha_buf;
}

void Fl_Gl_Window_Driver::draw_string_legacy_get_list(const char* str, int n) {
  static std::vector<unsigned short> utf16_buf{};
  unsigned int wn = fl_utf8toUtf16(str, n, utf16_buf.data(), static_cast<unsigned int>(utf16_buf.size()));
  if (wn >= utf16_buf.size()) {
    utf16_buf.resize(static_cast<size_t>(wn + 1U));
    wn = fl_utf8toUtf16(str, n, utf16_buf.data(), static_cast<unsigned int>(utf16_buf.size()));
  }

  int size = 0;
  if (gl_start_scale != 1.0F) {
    size = fl_graphics_driver->font_descriptor()->size;
    gl_font(fl_font(), static_cast<Fl_Fontsize>(static_cast<float>(size) * gl_start_scale));
  }

  for (unsigned int i = 0; i < wn; ++i) {
    const unsigned int r = (static_cast<unsigned int>(utf16_buf[i]) & 0xFC00U) >> 10U;
    get_list(gl_fontsize, static_cast<int>(r));
  }

  glCallLists(static_cast<GLsizei>(wn), GL_UNSIGNED_SHORT, utf16_buf.data());

  if (gl_start_scale != 1.0F) {
    gl_font(fl_font(), size);
  }
}

void Fl_Gl_Window_Driver::draw_string_legacy_glut(const char* str, int n) {
  std::vector<uchar> str_nul(static_cast<size_t>(n + 1), 0U);
  int m = 0;
  for (int i = 0; i < n; ++i) {
    if (static_cast<uchar>(str[i]) < 128U) {
      str_nul[static_cast<size_t>(m++)] = static_cast<uchar>(str[i]);
    }
  }
  str_nul[static_cast<size_t>(m)] = 0U;
  n = m;

  Fl_Surface_Device::push_current(Fl_Display_Device::display_device());
  fl_graphics_driver->font_descriptor(gl_fontsize);
  const Fl_Gl_Window *gwin = Fl_Window::current()->as_gl_window();
  gl_scale = (gwin != nullptr) ? gwin->pixels_per_unit() : 1.0F;

  const auto *str_ptr = reinterpret_cast<const char*>(str_nul.data());
  const float stroke_len = static_cast<float>(glutStrokeLength(GLUT_STROKE_ROMAN, str_nul.data()));
  const float ratio = (stroke_len > 0.0F)
                      ? (static_cast<float>(fl_width(str_ptr, n)) * gl_scale / stroke_len)
                      : 1.0F;
  Fl_Surface_Device::pop_current();

  GLint matrixMode = 0;
  glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  const float winw = gl_scale * static_cast<float>(Fl_Window::current()->w());
  const float winh = gl_scale * static_cast<float>(Fl_Window::current()->h());

  std::array<GLfloat, 4> pos = {0.0F, 0.0F, 0.0F, 0.0F};
  glGetFloatv(GL_CURRENT_RASTER_POSITION, pos.data());
  if (gl_start_scale != 1.0F) {
    pos[0] /= gl_start_scale;
    pos[1] /= gl_start_scale;
  }

  const float R = 2.0F * ratio;
  glScalef(R / winw, R / winh, 1.0F);
  glTranslatef(-winw / R, -winh / R, 0.0F);
  glTranslatef(pos[0] * 2.0F / R, pos[1] * 2.0F / R, 0.0F);
  glutStrokeString(GLUT_STROKE_ROMAN, str_nul.data());

  glPopAttrib();
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(static_cast<GLenum>(matrixMode));

#if HAVE_GL_GLU_H
  const auto width = static_cast<float>(fl_width(str_ptr));
  pos[0] += width;
  std::array<GLdouble, 16> modelmat = {};
  glGetDoublev(GL_MODELVIEW_MATRIX, modelmat.data());
  std::array<GLdouble, 16> projmat = {};
  glGetDoublev(GL_PROJECTION_MATRIX, projmat.data());
  GLdouble objX = 0.0;
  GLdouble objY = 0.0;
  GLdouble objZ = 0.0;
  std::array<GLint, 4> viewport = {};
  glGetIntegerv(GL_VIEWPORT, viewport.data());
  (void)gluUnProject(static_cast<GLdouble>(pos[0]), static_cast<GLdouble>(pos[1]), static_cast<GLdouble>(pos[2]),
                     modelmat.data(), projmat.data(), viewport.data(), &objX, &objY, &objZ);

  if (gl_start_scale != 1.0F) {
    objX *= static_cast<double>(gl_start_scale);
    objY *= static_cast<double>(gl_start_scale);
  }
  glRasterPos2d(objX, objY);
#endif
}

#endif // HAVE_GL || defined(FL_DOXYGEN)