//
// System color support for the Fast Light Tool Kit (FLTK).
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

/** \file Fl_get_system_colors.cxx
 \brief System color support
*/

#include <FL/Fl.H>
#include "Fl_Screen_Driver.H"
#include "Fl_System_Driver.H"
#include <FL/platform.H>
#include <FL/fl_string_functions.h>
#include "flstring.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Tiled_Image.H>
#include "tile.xpm"

/**
    Changes fl_color(FL_BACKGROUND_COLOR) to the given color,
    and changes the gray ramp from 32 to 56 to black to white.  These are
    the colors used as backgrounds by almost all widgets and used to draw
    the edges of all the boxtypes.
*/
void Fl::background(const uchar r, const uchar g, const uchar b) {
  Fl_Screen_Driver::bg_set = 1;

  // replace the gray ramp so that FL_GRAY is this color
  uchar adj_r = r;
  if (adj_r == 0U) {
    adj_r = 1U;
  } else if (adj_r == 255U) {
    adj_r = 254U;
  } else {
    // Value remains in [1, 254]
  }
  const double powr = std::log(static_cast<double>(adj_r) / 255.0) /
                      std::log(static_cast<double>(FL_GRAY - FL_GRAY_RAMP) / (static_cast<double>(FL_NUM_GRAY) - 1.0));

  uchar adj_g = g;
  if (adj_g == 0U) {
    adj_g = 1U;
  } else if (adj_g == 255U) {
    adj_g = 254U;
  } else {
    // Value remains in [1, 254]
  }
  const double powg = std::log(static_cast<double>(adj_g) / 255.0) /
                      std::log(static_cast<double>(FL_GRAY - FL_GRAY_RAMP) / (static_cast<double>(FL_NUM_GRAY) - 1.0));

  uchar adj_b = b;
  if (adj_b == 0U) {
    adj_b = 1U;
  } else if (adj_b == 255U) {
    adj_b = 254U;
  } else {
    // Value remains in [1, 254]
  }
  const double powb = std::log(static_cast<double>(adj_b) / 255.0) /
                      std::log(static_cast<double>(FL_GRAY - FL_GRAY_RAMP) / (static_cast<double>(FL_NUM_GRAY) - 1.0));

  for (int i = 0; i < FL_NUM_GRAY; ++i) {
    const double gray = static_cast<double>(i) / (static_cast<double>(FL_NUM_GRAY) - 1.0);
    set_color(fl_gray_ramp(i),
                  static_cast<uchar>(std::lround(std::pow(gray, powr) * 255.0)),
                  static_cast<uchar>(std::lround(std::pow(gray, powg) * 255.0)),
                  static_cast<uchar>(std::lround(std::pow(gray, powb) * 255.0)));
  }
}


/** Changes fl_color(FL_FOREGROUND_COLOR). */
void Fl::foreground(const uchar r, const uchar g, const uchar b) {
  Fl_Screen_Driver::fg_set = 1;

  set_color(FL_FOREGROUND_COLOR, r, g, b);
}


/**
    Changes the alternative background color. This color is used as a
    background by Fl_Input and other text widgets.
    <P>This call may change fl_color(FL_FOREGROUND_COLOR) if it
    does not provide sufficient contrast to FL_BACKGROUND2_COLOR.
*/
void Fl::background2(const uchar r, const uchar g, const uchar b) {
  Fl_Screen_Driver::bg2_set = 1;

  set_color(FL_BACKGROUND2_COLOR, r, g, b);
  set_color(FL_FOREGROUND_COLOR,
                get_color(fl_contrast(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR)));
}


// these are set by Fl::args() and override any system colors:
const char *fl_fg = nullptr;
const char *fl_bg = nullptr;
const char *fl_bg2 = nullptr;

/**
 Parse a string containing a description of a color and write r, g, and b.

 This call is used by the Pixmap file format interpreter and by the command
 line arguments parser to set UI colors.

 RGB color triplets usually start with a '#' character, but it can be omitted
 if it does not conflict with the later rules. Color components are defined
 in hexadecimal notation with 1, 2, 3, or four hex digits per component, making
 color triplets 3, 6, 9, or 12 characters long. The interpreter is case
 insensitive. Valid code examples include "FF0000" for red, "#0F0" for green,
 and "000000004444" for a dark blue.

 On the X11 platform, color values can also be given a color name like "red".
 The list of accepted color names is provided by the X11 server.

 If none of the color interpretations work, \ref fl_parse_color returns 0.
 The Pixmap reader interprets those as transparent, and are usually written as
 "None", "#transparent", or "bg".

 \param[in] p a C-string describing the color
 \param[out] r, g, b the color components in the 0...255 range
 \return 0 if the color cannot be interpreted, 1 otherwise
 */
int fl_parse_color(const char* const p, uchar& r, uchar& g, uchar& b) {
  return Fl::screen_driver()->parse_color(p, r, g, b);
}


/** \fn Fl::get_system_colors()
    Read the user preference colors from the system and use them to call
    Fl::foreground(), Fl::background(), and Fl::background2().

    This is done by Fl_Window::show(argc,argv) before applying
    the -fg and -bg switches.

    On X this reads some common values from the Xdefaults database.
    KDE users can set these values by running the "krdb" program, and
    newer versions of KDE set this automatically if you check the "apply
    style to other X programs" switch in their control panel.
*/
void Fl::get_system_colors()
{
  screen_driver()->get_system_colors();
}


//// Simple implementation of 2.0 Fl::scheme() interface...
#define D1 BORDER_WIDTH
#define D2 (BORDER_WIDTH+BORDER_WIDTH)

extern void     fl_up_box(int, int, int, int, Fl_Color);
extern void     fl_down_box(int, int, int, int, Fl_Color);
extern void     fl_thin_up_box(int, int, int, int, Fl_Color);
extern void     fl_thin_down_box(int, int, int, int, Fl_Color);
extern void     fl_round_up_box(int, int, int, int, Fl_Color);
extern void     fl_round_down_box(int, int, int, int, Fl_Color);
extern void     fl_round_focus(Fl_Boxtype, int, int, int, int, Fl_Color, Fl_Color);

extern void     fl_up_frame(int, int, int, int, Fl_Color);
extern void     fl_down_frame(int, int, int, int, Fl_Color);
extern void     fl_thin_up_frame(int, int, int, int, Fl_Color);
extern void     fl_thin_down_frame(int, int, int, int, Fl_Color);

#ifndef FL_DOXYGEN
const char      *Fl::scheme_ = nullptr;     // current scheme
Fl_Image        *Fl::scheme_bg_ = nullptr;    // current background image for the scheme
#endif

// Lazy getter function for the static pixmap tile to avoid static init exception issues
static Fl_Pixmap* get_tile_pixmap() {
  static Fl_Pixmap tile_instance(tile_xpm);
  return &tile_instance;
}

/**
  Sets the current widget scheme. NULL will use the scheme defined
  in the FLTK_SCHEME environment variable or the scheme resource
  under X11. Otherwise, any of the following schemes can be used:

    - "none" - This is the default look-n-feel which resembles old
               Windows (95/98/Me/NT/2000) and old GTK/KDE

    - "base" - This is an alias for "none"

    - "plastic" - This scheme is inspired by the Aqua user interface
                  on macOS

    - "gtk+" - This scheme is inspired by the Red Hat Bluecurve theme

    - "gleam" - This scheme is inspired by the Clearlooks Glossy scheme.
                (Colin Jones and Edmanuel Torres).

    - "oxy" - This is a subset of Dmitrij K's oxy scheme (STR 2675, 3477)

  If the given scheme name is unknown, the default scheme will be used.

  Setting the scheme (name) is case insensitive, but the stored scheme name will
  always be lowercase and Fl::scheme() will return this lowercase name or
  \p NULL if no scheme or the default scheme ("none" or "base") was set.

  \param[in]  name   Scheme name of NULL

  \retval     0 if the scheme has not been set or is the default scheme
  \retval     1 if a scheme other than "none"/"base" was set

  \see Fl::scheme() to get the name of the current scheme
  \see Fl::is_scheme(const char*) to test if the specified scheme is set
*/
int Fl::scheme(const char * const name) {
  const char *s = name;
  if (s == nullptr) {
    s = screen_driver()->get_system_scheme();
  }

  if (s != nullptr) {
    if ((fl_ascii_strcasecmp(s, "none") == 0) || (fl_ascii_strcasecmp(s, "base") == 0) || (*s == '\0')) {
      s = nullptr;
    } else if (fl_ascii_strcasecmp(s, "gtk+") == 0) {
      const char * allocated_scheme = fl_strdup("gtk+");
      s = allocated_scheme;
    } else if (fl_ascii_strcasecmp(s, "plastic") == 0) {
      const char * allocated_scheme = fl_strdup("plastic");
      s = allocated_scheme;
    } else if (fl_ascii_strcasecmp(s, "gleam") == 0) {
      const char * allocated_scheme = fl_strdup("gleam");
      s = allocated_scheme;
    } else if (fl_ascii_strcasecmp(s, "oxy") == 0) {
      const char * allocated_scheme = fl_strdup("oxy");
      s = allocated_scheme;
    } else {
      s = nullptr;
    }
  }

  if (scheme_ != nullptr) {
    std::free(static_cast<void*>(const_cast<char*>(scheme_)));
  }
  scheme_ = s;

  // Save the new scheme in the FLTK_SCHEME env var so that child processes
  // inherit it...
  char env_buf[1024] = "FLTK_SCHEME=";
  if (s != nullptr) {
    static_cast<void>(strlcat(&env_buf[0], s, sizeof(env_buf)));
  }
  static_cast<void>(Fl::system_driver()->putenv(&env_buf[0]));

  // Load the scheme...
  static_cast<void>(reload_scheme());
  return (s != nullptr) ? 1 : 0;
}

/**
  Called internally when setting a new scheme according to scheme name.
  Loads or reloads the current scheme selection.

  \return   Always 1 (this may change in the future)

  See void Fl::scheme(const char *name)

  \internal
  \note Internal: Should this method be private?
*/
int Fl::reload_scheme() {
  if ((scheme_ != nullptr) && (fl_ascii_strcasecmp(scheme_, "plastic") == 0)) {
    // Update the tile image to match the background color...
    uchar r = 0U;
    uchar g = 0U;
    uchar b = 0U;
    // static uchar levels[3] = { 0xff, 0xef, 0xe8 };
    // OSX 10.3 and higher use a background with less contrast...
    constexpr uchar levels[3] = { 0xffU, 0xf8U, 0xf4U };

    get_color(FL_GRAY, r, g, b);

    // printf("FL_GRAY = 0x%02x 0x%02x 0x%02x\n", r, g, b);

    constexpr char glyphs[4] = "Oo.";

    // Unrolled loop to satisfy static array bounds checking without warnings
    int nr0 = static_cast<int>(static_cast<unsigned int>(levels[0]) * static_cast<unsigned int>(r) / 0xe8U);
    if (nr0 > 255) { nr0 = 255; }
    int ng0 = static_cast<int>(static_cast<unsigned int>(levels[0]) * static_cast<unsigned int>(g) / 0xe8U);
    if (ng0 > 255) { ng0 = 255; }
    int nb0 = static_cast<int>(static_cast<unsigned int>(levels[0]) * static_cast<unsigned int>(b) / 0xe8U);
    if (nb0 > 255) { nb0 = 255; }
    const int c_nr0 = nr0;
    const int c_ng0 = ng0;
    const int c_nb0 = nb0;
    static_cast<void>(std::snprintf(&tile_cmap[0][0], sizeof(tile_cmap[0]), "%c c #%02x%02x%02x", glyphs[0], c_nr0, c_ng0, c_nb0));

    int nr1 = static_cast<int>(static_cast<unsigned int>(levels[1]) * static_cast<unsigned int>(r) / 0xe8U);
    if (nr1 > 255) { nr1 = 255; }
    int ng1 = static_cast<int>(static_cast<unsigned int>(levels[1]) * static_cast<unsigned int>(g) / 0xe8U);
    if (ng1 > 255) { ng1 = 255; }
    int nb1 = static_cast<int>(static_cast<unsigned int>(levels[1]) * static_cast<unsigned int>(b) / 0xe8U);
    if (nb1 > 255) { nb1 = 255; }
    const int c_nr1 = nr1;
    const int c_ng1 = ng1;
    const int c_nb1 = nb1;
    static_cast<void>(std::snprintf(&tile_cmap[1][0], sizeof(tile_cmap[1]), "%c c #%02x%02x%02x", glyphs[1], c_nr1, c_ng1, c_nb1));

    int nr2 = static_cast<int>(static_cast<unsigned int>(levels[2]) * static_cast<unsigned int>(r) / 0xe8U);
    if (nr2 > 255) { nr2 = 255; }
    int ng2 = static_cast<int>(static_cast<unsigned int>(levels[2]) * static_cast<unsigned int>(g) / 0xe8U);
    if (ng2 > 255) { ng2 = 255; }
    int nb2 = static_cast<int>(static_cast<unsigned int>(levels[2]) * static_cast<unsigned int>(b) / 0xe8U);
    if (nb2 > 255) { nb2 = 255; }
    const int c_nr2 = nr2;
    const int c_ng2 = ng2;
    const int c_nb2 = nb2;
    static_cast<void>(std::snprintf(&tile_cmap[2][0], sizeof(tile_cmap[2]), "%c c #%02x%02x%02x", glyphs[2], c_nr2, c_ng2, c_nb2));

    Fl_Pixmap* const tile_pixmap = get_tile_pixmap();
    tile_pixmap->uncache();

    if (scheme_bg_ == nullptr) {
      scheme_bg_ = new Fl_Tiled_Image(tile_pixmap, 0, 0);
    }

    // Load plastic buttons, etc...
    set_boxtype(FL_UP_FRAME,        FL_PLASTIC_UP_FRAME);
    set_boxtype(FL_DOWN_FRAME,      FL_PLASTIC_DOWN_FRAME);
    set_boxtype(FL_THIN_UP_FRAME,   FL_PLASTIC_UP_FRAME);
    set_boxtype(FL_THIN_DOWN_FRAME, FL_PLASTIC_DOWN_FRAME);

    set_boxtype(FL_UP_BOX,          FL_PLASTIC_UP_BOX);
    set_boxtype(FL_DOWN_BOX,        FL_PLASTIC_DOWN_BOX);
    set_boxtype(FL_THIN_UP_BOX,     FL_PLASTIC_THIN_UP_BOX);
    set_boxtype(FL_THIN_DOWN_BOX,   FL_PLASTIC_THIN_DOWN_BOX);
    set_boxtype(FL_ROUND_UP_BOX,    FL_PLASTIC_ROUND_UP_BOX);
    set_boxtype(FL_ROUND_DOWN_BOX,  FL_PLASTIC_ROUND_DOWN_BOX);

    // Use standard size scrollbars...
    Fl::scrollbar_size(16);
  } else if ((scheme_ != nullptr) && (
             (fl_ascii_strcasecmp(scheme_, "gtk+") == 0) ||
             (fl_ascii_strcasecmp(scheme_, "gleam") == 0) ||
             (fl_ascii_strcasecmp(scheme_, "oxy") == 0))) {
    // Shared cleanup for GTK+, Gleam, and Oxy
    if (scheme_bg_ != nullptr) {
      delete scheme_bg_;
      scheme_bg_ = nullptr;
    }

    if (fl_ascii_strcasecmp(scheme_, "gtk+") == 0) {
      set_boxtype(FL_UP_FRAME,        FL_GTK_UP_FRAME);
      set_boxtype(FL_DOWN_FRAME,      FL_GTK_DOWN_FRAME);
      set_boxtype(FL_THIN_UP_FRAME,   FL_GTK_THIN_UP_FRAME);
      set_boxtype(FL_THIN_DOWN_FRAME, FL_GTK_THIN_DOWN_FRAME);

      set_boxtype(FL_UP_BOX,          FL_GTK_UP_BOX);
      set_boxtype(FL_DOWN_BOX,        FL_GTK_DOWN_BOX);
      set_boxtype(FL_THIN_UP_BOX,     FL_GTK_THIN_UP_BOX);
      set_boxtype(FL_THIN_DOWN_BOX,   FL_GTK_THIN_DOWN_BOX);
      set_boxtype(FL_ROUND_UP_BOX,    FL_GTK_ROUND_UP_BOX);
      set_boxtype(FL_ROUND_DOWN_BOX,  FL_GTK_ROUND_DOWN_BOX);
    } else if (fl_ascii_strcasecmp(scheme_, "gleam") == 0) {
      set_boxtype(FL_UP_FRAME,        FL_GLEAM_UP_FRAME);
      set_boxtype(FL_DOWN_FRAME,      FL_GLEAM_DOWN_FRAME);
      set_boxtype(FL_THIN_UP_FRAME,   FL_GLEAM_UP_FRAME);
      set_boxtype(FL_THIN_DOWN_FRAME, FL_GLEAM_DOWN_FRAME);

      set_boxtype(FL_UP_BOX,          FL_GLEAM_UP_BOX);
      set_boxtype(FL_DOWN_BOX,        FL_GLEAM_DOWN_BOX);
      set_boxtype(FL_THIN_UP_BOX,     FL_GLEAM_THIN_UP_BOX);
      set_boxtype(FL_THIN_DOWN_BOX,   FL_GLEAM_THIN_DOWN_BOX);
      set_boxtype(FL_ROUND_UP_BOX,    FL_GLEAM_ROUND_UP_BOX);
      set_boxtype(FL_ROUND_DOWN_BOX,  FL_GLEAM_ROUND_DOWN_BOX);
    } else { // "oxy"
      set_boxtype(FL_UP_FRAME,        FL_OXY_UP_FRAME);
      set_boxtype(FL_DOWN_FRAME,      FL_OXY_DOWN_FRAME);
      set_boxtype(FL_THIN_UP_FRAME,   FL_OXY_THIN_UP_FRAME);
      set_boxtype(FL_THIN_DOWN_FRAME, FL_OXY_THIN_DOWN_FRAME);

      set_boxtype(FL_UP_BOX,          FL_OXY_UP_BOX);
      set_boxtype(FL_DOWN_BOX,        FL_OXY_DOWN_BOX);
      set_boxtype(FL_THIN_UP_BOX,     FL_OXY_THIN_UP_BOX);
      set_boxtype(FL_THIN_DOWN_BOX,   FL_OXY_THIN_DOWN_BOX);
      set_boxtype(FL_ROUND_UP_BOX,    FL_OXY_ROUND_UP_BOX);
      set_boxtype(FL_ROUND_DOWN_BOX,  FL_OXY_ROUND_DOWN_BOX);
    }

    // Use slightly thinner scrollbars for GTK+, Gleam, and Oxy...
    scrollbar_size(15);
  } else {
    // Use the standard FLTK look-n-feel...
    if (scheme_bg_ != nullptr) {
      delete scheme_bg_;
      scheme_bg_ = nullptr;
    }

    set_boxtype(FL_UP_FRAME,        fl_up_frame, D1, D1, D2, D2);
    set_boxtype(FL_DOWN_FRAME,      fl_down_frame, D1, D1, D2, D2);
    set_boxtype(FL_THIN_UP_FRAME,   fl_thin_up_frame, 1, 1, 2, 2);
    set_boxtype(FL_THIN_DOWN_FRAME, fl_thin_down_frame, 1, 1, 2, 2);

    set_boxtype(FL_UP_BOX,          fl_up_box, D1, D1, D2, D2);
    set_boxtype(FL_DOWN_BOX,        fl_down_box, D1, D1, D2, D2);
    set_boxtype(FL_THIN_UP_BOX,     fl_thin_up_box, 1, 1, 2, 2);
    set_boxtype(FL_THIN_DOWN_BOX,   fl_thin_down_box, 1, 1, 2, 2);
    set_boxtype(FL_ROUND_UP_BOX,    fl_round_up_box, 3, 3, 6, 6, fl_round_focus);
    set_boxtype(FL_ROUND_DOWN_BOX,  fl_round_down_box, 3, 3, 6, 6, fl_round_focus);

    // Use standard size scrollbars...
    Fl::scrollbar_size(16);
  }

  // Set (or clear) the background tile for all windows...
  for (Fl_Window *win = first_window(); win != nullptr; win = next_window(win)) {
    win->labeltype(scheme_bg_ != nullptr ? FL_NORMAL_LABEL : FL_NO_LABEL);
    win->align(static_cast<unsigned int>(FL_ALIGN_CENTER) |
               static_cast<unsigned int>(FL_ALIGN_INSIDE) |
               static_cast<unsigned int>(FL_ALIGN_CLIP));
    win->image(scheme_bg_);
    win->redraw();
  }

  return 1;
}