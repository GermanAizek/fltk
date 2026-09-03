//
// Clock widget for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2017 by Bill Spitzak and others.
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
#include <FL/Fl_Clock.H>
#include <FL/Fl_Round_Clock.H>
#include "Fl_System_Driver.H"
#include <FL/fl_draw.H>
#include <math.h>
#include <time.h>

// Original clock display written by Paul Haeberli at SGI.
// Modifications by Mark Overmars for Forms
// Further changes by Bill Spitzak for fltk

namespace {
  constexpr float hourhand[4][2] = {{-0.5F, 0.0F}, {0.0F, 1.5F}, {0.5F, 0.0F}, {0.0F, -7.0F}};
  constexpr float  minhand[4][2] = {{-0.5F, 0.0F}, {0.0F, 1.5F}, {0.5F, 0.0F}, {0.0F, -11.5F}};
  constexpr float  sechand[4][2] = {{-0.1F, 0.0F}, {0.0F, 2.0F}, {0.1F, 0.0F}, {0.0F, -11.5F}};

  static void drawhand(double ang, const float v[4][2], Fl_Color fill, Fl_Color line)
  {
    fl_push_matrix();
    fl_rotate(ang);
    fl_color(fill);
    fl_begin_polygon();
    for (int i = 0; i < 4; ++i) {
      fl_vertex(v[i][0], v[i][1]);
    }
    fl_end_polygon();
    fl_color(line);
    fl_begin_loop();
    for (int i = 0; i < 4; ++i) {
      fl_vertex(v[i][0], v[i][1]);
    }
    fl_end_loop();
    fl_pop_matrix();
  }

  static void rect(double x_coord, double y_coord, double w_size, double h_size) {
    const double r = x_coord + w_size;
    const double t = y_coord + h_size;
    fl_begin_polygon();
    fl_vertex(x_coord, y_coord);
    fl_vertex(r, y_coord);
    fl_vertex(r, t);
    fl_vertex(x_coord, t);
    fl_end_polygon();
  }
} // namespace

void Fl_Clock_Output::drawhands(Fl_Color fill_col, Fl_Color line_col) const {
  Fl_Color f = fill_col;
  Fl_Color l = line_col;
  if (active_r() == 0) {
    f = fl_inactive(f);
    l = fl_inactive(l);
  }
  drawhand(-360.0 * (static_cast<double>(hour()) + (static_cast<double>(minute()) / 60.0)) / 12.0, &hourhand[0], f, l);
  drawhand(-360.0 * (static_cast<double>(minute()) + (static_cast<double>(second()) / 60.0)) / 60.0, &minhand[0], f, l);
  drawhand(-360.0 * (static_cast<double>(second()) / 60.0), &sechand[0], f, l);
}

/**
  Draw clock with the given position and size.
  \param[in] X, Y, W, H position and size
*/
void Fl_Clock_Output::draw(int X, int Y, int W, int H) const {
  const Fl_Color box_color = (static_cast<int>(type()) == FL_ROUND_CLOCK) ? FL_GRAY : color();
  draw_box(box(), X, Y, W, H, box_color);
  fl_push_matrix();
  fl_translate(static_cast<double>(X) + (static_cast<double>(W) / 2.0) - 0.5, static_cast<double>(Y) + (static_cast<double>(H) / 2.0) - 0.5);
  fl_scale(static_cast<double>(W - 1) / 28.0, static_cast<double>(H - 1) / 28.0);
  if (static_cast<int>(type()) == FL_ROUND_CLOCK) {
    fl_color((active_r() != 0) ? color() : fl_inactive(color()));
    fl_begin_polygon();
    fl_circle(0.0, 0.0, 14.0);
    fl_end_polygon();
    fl_color((active_r() != 0) ? FL_FOREGROUND_COLOR : fl_inactive(FL_FOREGROUND_COLOR));
    fl_begin_loop();
    fl_circle(0.0, 0.0, 14.0);
    fl_end_loop();
  }

  // draw the shadows:
  if (shadow_ != 0) {
    const Fl_Color shadow_color = fl_color_average(box_color, FL_BLACK, 0.5F);
    fl_push_matrix();
    fl_translate(0.60, 0.60);
    drawhands(shadow_color, shadow_color);
    fl_pop_matrix();
  }

  // draw the tick marks:
  fl_push_matrix();
  fl_color((active_r() != 0) ? FL_FOREGROUND_COLOR : fl_inactive(FL_FOREGROUND_COLOR));
  for (int i = 0; i < 12; ++i) {
    if (i == 6) {
      rect(-0.5, 9.0, 1.0, 2.0);
    } else if ((i == 3) || (i == 0) || (i == 9)) {
      rect(-0.5, 9.5, 1.0, 1.0);
    } else {
      rect(-0.25, 9.5, 0.5, 1.0);
    }
    fl_rotate(-30.0);
  }
  fl_pop_matrix();

  // draw the hands:
  drawhands(selection_color(), FL_FOREGROUND_COLOR); // color was 54
  fl_pop_matrix();
}

/**
  Draw clock with current position and size.
*/
void Fl_Clock_Output::draw() {
  draw(x(), y(), w(), h());
  draw_label();
}

/**
  Set the displayed time.
  Set the time in hours, minutes, and seconds.
  \param[in] H, m, s displayed time
  \see hour(), minute(), second()
 */
void Fl_Clock_Output::value(int H, int m, int s) {
  if ((H != hour_) || (m != minute_) || (s != second_)) {
    hour_ = static_cast<uchar>(H);
    minute_ = static_cast<uchar>(m);
    second_ = static_cast<uchar>(s);
    value_ = static_cast<unsigned long>((static_cast<unsigned int>(H) * 3600U) + (static_cast<unsigned int>(m) * 60U) + static_cast<unsigned int>(s));
    damage(FL_DAMAGE_CHILD);
  }
}

/**
  Set the displayed time.
  Set the time in seconds since the UNIX epoch (January 1, 1970).
  \param[in] v seconds since epoch
  \see value()
 */
void Fl_Clock_Output::value(unsigned long v) {
  value_ = v;
  time_t vv = static_cast<time_t>(v);
  const struct tm * const timeofday = localtime(&vv);
  if (timeofday != nullptr) {
    value(timeofday->tm_hour, timeofday->tm_min, timeofday->tm_sec);
  }
}

/**
  Create a new Fl_Clock_Output widget with the given position, size and label.

  The default clock type is \c FL_SQUARE_CLOCK and the default boxtype is
  \c FL_UP_BOX.

  \param[in] X, Y, W, H position and size of the widget
  \param[in] L widget label, default is no label
 */
Fl_Clock_Output::Fl_Clock_Output(int X, int Y, int W, int H, const char *L)
: Fl_Widget(X, Y, W, H, L),
  value_(0UL),
  hour_(0),
  minute_(0),
  second_(0),
  shadow_(1) {
  box(FL_UP_BOX);
  selection_color(fl_gray_ramp(5));
  align(FL_ALIGN_BOTTOM);
}

////////////////////////////////////////////////////////////////

/**
  Create an Fl_Clock widget using the given position, size, and label string.

  The default clock type is FL_SQUARE_CLOCK and the default
  boxtype is \c FL_UP_BOX.

  \param[in] X, Y, W, H position and size of the widget
  \param[in] L widget label, default is no label
 */
Fl_Clock::Fl_Clock(int X, int Y, int W, int H, const char *L)
  : Fl_Clock_Output(X, Y, W, H, L) {}

/**
  Create an Fl_Clock widget using the given clock type \p t,
  position, size, and label string.

  The default clock type \p t is \c FL_SQUARE_CLOCK. You can set the
  clock type to FL_ROUND_CLOCK or any other valid clock type.
  See Fl_Clock_Output widget for applicable values.

  The default boxtype is \c FL_UP_BOX for \c FL_SQUARE_CLOCK
  and \c FL_NO_BOX for \c FL_ROUND_CLOCK, if set by the constructor.
  If you change the clock type with type() later you should also set
  the boxtype with box().

  \param[in] t type of clock: FL_ROUND_CLOCK or FL_SQUARE_CLOCK (0)
  \param[in] X, Y, W, H position and size of the widget
  \param[in] L widget label, default is no label

  \see class Fl_Clock_Output
*/

Fl_Clock::Fl_Clock(unsigned char t, int X, int Y, int W, int H, const char *L)
  : Fl_Clock_Output(X, Y, W, H, L) {
  type(t);
  box((static_cast<int>(t) == FL_ROUND_CLOCK) ? FL_NO_BOX : FL_UP_BOX);
}

static void tick(void *v) {
  if (v != nullptr) {
    time_t sec = 0;
    int usec = 0;
    Fl::system_driver()->gettime(&sec, &usec);
    double delta = static_cast<double>(1000000 - usec) / 1000000.0; // time till next second
    // if current time is just before full second, show that full second
    // and wait one more second (STR 3516)
    if (delta < 0.1) {
      delta += 1.0;
      sec++;
    }
    Fl_Clock* const clk = static_cast<Fl_Clock*>(v);
    clk->value(static_cast<unsigned long>(sec));

    Fl::add_timeout(delta, tick, v);
  }
}

int Fl_Clock::handle(int event) {
  switch (event) {
    case FL_SHOW:
      tick(this);
      break;
    case FL_HIDE:
      Fl::remove_timeout(tick, this);
      break;
    default:
      break;
  }
  return Fl_Clock_Output::handle(event);
}

/**
  The destructor removes the clock.
 */
Fl_Clock::~Fl_Clock() {
  Fl::remove_timeout(tick, this);
}


/**
  Create an Fl_Round_Clock widget using the given
  position, size, and label string.

  The clock type is \c FL_ROUND_CLOCK and the boxtype is \c FL_NO_BOX.

  This construcktor is the same as Fl_Clock(FL_ROUND_CLOCK, X, Y, W, H, L).
  \see Fl_Clock(uchar, int, int, int, int, const char *)

  \param[in] X, Y, W, H position and size of the widget
  \param[in] L widget label, default is no label
*/

Fl_Round_Clock::Fl_Round_Clock(int X, int Y, int W, int H, const char *L)
: Fl_Clock(X, Y, W, H, L)
{
  type(FL_ROUND_CLOCK);
  box(FL_NO_BOX);
}
