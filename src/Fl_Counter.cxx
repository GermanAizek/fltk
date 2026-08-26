//
// Counter widget for the Fast Light Tool Kit (FLTK).
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
#include <FL/Fl_Counter.H>
#include <FL/Fl_Simple_Counter.H>
#include <FL/fl_draw.H>
#include <string>
#include <cmath>

namespace {
  // This struct describes the four arrow boxes
  struct arrow_box {
    int width;
    Fl_Arrow_Type arrow_type;
    Fl_Boxtype boxtype;
    Fl_Orientation orientation;
    arrow_box() : width(0), arrow_type(FL_ARROW_SINGLE), boxtype(FL_NO_BOX), orientation(FL_ORIENT_RIGHT) {}
  };

  constexpr double INITIALREPEAT = 0.5;
  constexpr double REPEAT = 0.1;
} // namespace

/**
  Compute sizes (widths) of arrow boxes.

  This method computes the two sizes of the arrow boxes of Fl_Counter.
  You can override it in a subclass if you want to draw fancy arrows
  or change the layout. However, the basic layout is fixed and can't
  be changed w/o overriding the draw() and handle() methods.

  Basic layout:
  \code
    +------+-----+-------------+-----+------+
    |  <<  |  <  |    value    |  >  |  >>  |
    +------+-----+-------------+-----+------+
  \endcode

  The returned value \p w2 should be zero if the counter type() is FL_SIMPLE_COUNTER.

  \param[out]   w1  width of single arrow box
  \param[out]   w2  width of double arrow box
*/
void Fl_Counter::arrow_widths(int &w1, int &w2) const {
  if (static_cast<int>(type()) == FL_SIMPLE_COUNTER) {
    w1 = (w() * 20) / 100;
    w2 = 0;
  } else {
    w1 = (w() * 13) / 100;
    w2 = (w() * 17) / 100;
  }
  // limit arrow box sizes to reserve more space for the text box
  if (w1 > 13) {
    w1 = 13;
  }
  if (w2 > 24) {
    w2 = 24;
  }
}

void Fl_Counter::draw() {
  arrow_box ab[4];

  // text box setup
  Fl_Boxtype tbt = box();
  if (tbt == FL_UP_BOX) {
    tbt = FL_DOWN_BOX;
  }
  if (tbt == FL_THIN_UP_BOX) {
    tbt = FL_THIN_DOWN_BOX;
  }

  // arrow boxes
  for (int i = 0; i < 4; i++) {
    if (static_cast<int>(mouseobj_) == (i + 1)) {
      ab[i].boxtype = fl_down(box());
    } else {
      ab[i].boxtype = box();
    }
  }

  ab[0].arrow_type = FL_ARROW_DOUBLE;
  ab[3].arrow_type = FL_ARROW_DOUBLE;      // first and last arrow
  ab[0].orientation = FL_ORIENT_LEFT;
  ab[1].orientation = FL_ORIENT_LEFT;     // left arrows

  int w1 = 0;
  int w2 = 0;
  arrow_widths(w1, w2);
  if (static_cast<int>(type()) == FL_SIMPLE_COUNTER) {
    w2 = 0;
  }

  ab[0].width = w2;
  ab[3].width = w2;          // double arrows
  ab[1].width = w1;
  ab[2].width = w1;          // single arrows

  const int tw = w() - (2 * (w1 + w2)); // text box width
  const int tx = x() + w1 + w2;         // text box position

  // always draw text box and text
  draw_box(tbt, tx, y(), tw, h(), FL_BACKGROUND2_COLOR);
  fl_font(textfont(), textsize());
  fl_color(active_r() ? textcolor() : fl_inactive(textcolor()));
  const std::string str = format_str();
  fl_draw(str.c_str(), tx, y(), tw, h(), FL_ALIGN_CENTER);
  if (Fl::focus() == this) {
    draw_focus(tbt, tx, y(), tw, h());
  }
  if ((static_cast<unsigned int>(damage()) & static_cast<unsigned int>(FL_DAMAGE_ALL)) != 0U) {
    Fl_Color arrow_color;
    if (active_r() != 0) {
      arrow_color = labelcolor();
    } else {
      arrow_color = fl_inactive(labelcolor());
    }

    // draw arrow boxes
    int xo = x();
    for (int i = 0; i < 4; i++) {
      if (ab[i].width > 0) {
        draw_box(ab[i].boxtype, xo, y(), ab[i].width, h(), color());
        const Fl_Rect bb(xo, y(), ab[i].width, h(), ab[i].boxtype);
        fl_draw_arrow(bb, ab[i].arrow_type, ab[i].orientation, arrow_color);
        xo += ab[i].width;
      }
      if (i == 1) {
        xo += tw;
      }
    }
  }
} // draw()

void Fl_Counter::increment_cb() {
  if (mouseobj_ != 0U) {
    double v = value();
    switch (static_cast<int>(mouseobj_)) {
      case 1:
        v -= lstep_;
        break;
      case 2:
        v = increment(v, -1);
        break;
      case 3:
        v = increment(v, 1);
        break;
      case 4:
        v += lstep_;
        break;
      default:
        break;
    }
    handle_drag(clamp(std::round(v)));
  }
}

void Fl_Counter::repeat_callback(void* data) {
  if (data != nullptr) {
    Fl_Counter* const b = static_cast<Fl_Counter*>(data);
    const int buttons = static_cast<int>(Fl::event_state()) & static_cast<int>(FL_BUTTONS); // any mouse button pressed
    const bool focus = (Fl::focus() == b);               // the widget has focus
    if ((b->mouseobj_ != 0U) && (buttons != 0) && focus) {
      Fl::add_timeout(REPEAT, repeat_callback, b);
      b->increment_cb();
    }
  }
}

int Fl_Counter::calc_mouseobj() const {
  int ret = -1;
  if (static_cast<int>(type()) == FL_NORMAL_COUNTER) {
    const int W = (w() * 15) / 100;
    if (Fl::event_inside(x(), y(), W, h()) != 0) {
      ret = 1;
    } else if (Fl::event_inside(x() + W, y(), W, h()) != 0) {
      ret = 2;
    } else if (Fl::event_inside(x() + w() - (2 * W), y(), W, h()) != 0) {
      ret = 3;
    } else if (Fl::event_inside(x() + w() - W, y(), W, h()) != 0) {
      ret = 4;
    } else {
      // nothing
    }
  } else {
    const int W = (w() * 20) / 100;
    if (Fl::event_inside(x(), y(), W, h()) != 0) {
      ret = 2;
    } else if (Fl::event_inside(x() + w() - W, y(), W, h()) != 0) {
      ret = 3;
    } else {
      // nothing
    }
  }
  return ret;
}

int Fl_Counter::handle(int event) {
  int ret = 0;
  switch (event) {
    case FL_RELEASE:
      if (mouseobj_ != 0U) {
        Fl::remove_timeout(repeat_callback, this);
        mouseobj_ = 0;
        redraw();
      }
      handle_release();
      ret = 1;
      break;
    case FL_PUSH:
      if (Fl::visible_focus() != 0) {
        Fl::focus(this);
      }
      {
        Fl_Widget_Tracker wp(this);
        handle_push();
        if (wp.deleted() != 0) {
          ret = 1;
          break;
        }
      }
      /* FALLTHROUGH */
    case FL_DRAG: {
      const int i = calc_mouseobj();
      if (static_cast<int>(mouseobj_) != i) {
        Fl::remove_timeout(repeat_callback, this);
        mouseobj_ = static_cast<unsigned char>(i);
        if (i > 0) {
          Fl::add_timeout(INITIALREPEAT, repeat_callback, this);
        }
        Fl_Widget_Tracker wp(this);
        increment_cb();
        if (wp.deleted() != 0) {
          ret = 1;
          break;
        }
        redraw();
      }
      ret = 1;
      break;
    }
    case FL_MOUSEWHEEL:
      handle_drag(clamp(increment(value(), (Fl::event_dy() - Fl::event_dx()) / 2)));
      ret = 1;
      break;
    case FL_KEYBOARD :
      switch (Fl::event_key()) {
        case FL_Left:
          handle_drag(clamp(increment(value(), -1)));
          ret = 1;
          break;
        case FL_Right:
          handle_drag(clamp(increment(value(), 1)));
          ret = 1;
          break;
        default:
          ret = 0;
          break;
      }
      break;
    case FL_UNFOCUS :
      mouseobj_ = 0;
      /* FALLTHROUGH */
    case FL_FOCUS :
      if (Fl::visible_focus() != 0) {
        redraw();
        ret = 1;
      } else {
        ret = 0;
      }
      break;
    case FL_ENTER : /* FALLTHROUGH */
    case FL_LEAVE :
      ret = 1;
      break;
    default:
      ret = 0;
      break;
  }
  return ret;
}

/**
  Destroys the valuator.
 */
Fl_Counter::~Fl_Counter() {
  Fl::remove_timeout(repeat_callback, this);
}

/**
  Creates a new Fl_Counter widget using the given position, size, and label
  string. The default type is FL_NORMAL_COUNTER.
  \param[in] X, Y, W, H position and size of the widget
  \param[in] L widget label, default is no label
 */
Fl_Counter::Fl_Counter(int X, int Y, int W, int H, const char* L)
  : Fl_Valuator(X, Y, W, H, L),
    lstep_(1.0),
    textfont_(FL_HELVETICA),
    textsize_(FL_NORMAL_SIZE),
    textcolor_(FL_FOREGROUND_COLOR),
    mouseobj_(0) {
  box(FL_UP_BOX);
  selection_color(FL_INACTIVE_COLOR); // was FL_BLUE
  align(FL_ALIGN_BOTTOM);
  bounds(-1000000.0, 1000000.0);
  Fl_Valuator::step(1, 10);
}


Fl_Simple_Counter::Fl_Simple_Counter(int X,int Y,int W,int H, const char *L)
: Fl_Counter(X,Y,W,H,L) {
  type(FL_SIMPLE_COUNTER);
}
