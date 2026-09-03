//
// Button widget for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file. If this
// file is missing or damaged, see the license at:
//
//     https://www.fltk.org/COPYING.php
//
// Please see the following page on how to report bugs and issues:
//
//     https://www.fltk.org/bugs.php
//

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include <FL/Fl_Radio_Button.H>
#include <FL/Fl_Toggle_Button.H>


Fl_Widget_Tracker *Fl_Button::key_release_tracker = nullptr;


// There are a lot of subclasses, named Fl_*_Button.  Some of
// them are implemented by setting the type() value and testing it
// here.  This includes Fl_Radio_Button and Fl_Toggle_Button

/**
 Sets the current value of the button.
 A non-zero value sets the button to 1 (ON), and zero sets it to 0 (OFF).
 \param[in] v button value.
 \see set(), clear()
 */
int Fl_Button::value(int v) {
  int ret = 0;
  const signed char new_v = (v != 0) ? 1 : 0;
  oldval = new_v;
  clear_changed();
  if (value_ != new_v) {
    value_ = new_v;
    if (box() != FL_NO_BOX) {
      redraw();
    } else {
      redraw_label();
    }
    ret = 1;
  }
  return ret;
}

/**
 Turns on this button and turns off all other radio buttons in the group
 (calling \c value(1) or \c set() does not do this).
 */
void Fl_Button::setonly() { // set this radio button on, turn others off
  (void)value(1);
  const Fl_Group* const g = parent();
  if (g != nullptr) {
    const int num_children = g->children();
    Fl_Widget* const* const a = g->array();
    for (int i = 0; i < num_children; ++i) {
      Fl_Widget* const o = a[i];
      if ((o != this) && (static_cast<int>(o->type()) == static_cast<int>(FL_RADIO_BUTTON))) {
        Fl_Button* const btn = dynamic_cast<Fl_Button*>(o);
        if (btn != nullptr) {
          (void)btn->value(0);
        }
      }
    }
  }
}

void Fl_Button::draw() {
  if (static_cast<int>(type()) != static_cast<int>(FL_HIDDEN_BUTTON)) {
    const Fl_Color col = (value_ != 0) ? selection_color() : color();
    const Fl_Boxtype bt = (value_ != 0) ? ((down_box() != FL_NO_BOX) ? down_box() : fl_down(box())) : box();
    const Fl_Group* const p = parent();
    if ((compact_ != 0U) && (p != nullptr)) {
      const int pw = p->w();
      const int ph = p->h();
      int px = 0;
      int py = 0;
      if (p->as_window() != nullptr) {
        px = 0;
        py = 0;
      } else {
        px = p->x();
        py = p->y();
      }
      fl_push_clip(x(), y(), w(), h());
      draw_box(bt, px, py, pw, ph, col);
      fl_pop_clip();
      Fl_Color divider_color = fl_gray_ramp(static_cast<int>(FL_NUM_GRAY) / 3);
      if (active_r() == 0) {
        divider_color = fl_inactive(divider_color);
      }
      if (x() + w() != px + pw) {
        constexpr int hh = 5;
        fl_color(divider_color);
        fl_yxline(x() + w() - 1, y() + hh, y() + h() - 1 - hh);
      }
      if (y() + h() != py + ph) {
        constexpr int ww = 5;
        fl_color(divider_color);
        fl_xyline(x() + ww, y() + h() - 1, x() + w() - 1 - ww);
      }
    } else {
      draw_box(bt, col);
    }
    draw_backdrop();
    if ((labeltype() == FL_NORMAL_LABEL) && (value_ != 0)) {
      const Fl_Color c = labelcolor();
      labelcolor(fl_contrast(c, col));
      draw_label();
      labelcolor(c);
    } else {
      draw_label();
    }
    if (Fl::focus() == this) {
      draw_focus();
    }
  }
}

int Fl_Button::handle(int event) {
  int ret = 0;
  
  auto handle_keyboard_trigger = [&]() -> int {
    int trigger_ret = 1;
    if (static_cast<unsigned char>(type()) == static_cast<unsigned char>(FL_RADIO_BUTTON)) {
      if (value_ == 0) {
        setonly();
        set_changed();
        if ((static_cast<unsigned int>(when()) & static_cast<unsigned int>(FL_WHEN_CHANGED)) != 0U) {
          do_callback(FL_REASON_CHANGED);
        } else if ((static_cast<unsigned int>(when()) & static_cast<unsigned int>(FL_WHEN_RELEASE)) != 0U) {
          do_callback(FL_REASON_RELEASED);
        } else {
          // MISRA compliance: final else clause
        }
      } else {
        if ((static_cast<unsigned int>(when()) & static_cast<unsigned int>(FL_WHEN_NOT_CHANGED)) != 0U) {
          do_callback(FL_REASON_SELECTED);
        }
      }
    } else if (static_cast<unsigned char>(type()) == static_cast<unsigned char>(FL_TOGGLE_BUTTON)) {
      (void)value((value() == 0) ? 1 : 0);
      set_changed();
      if ((static_cast<unsigned int>(when()) & static_cast<unsigned int>(FL_WHEN_CHANGED)) != 0U) {
        do_callback(FL_REASON_CHANGED);
      } else if ((static_cast<unsigned int>(when()) & static_cast<unsigned int>(FL_WHEN_RELEASE)) != 0U) {
        do_callback(FL_REASON_RELEASED);
      } else {
        // MISRA compliance: final else clause
      }
    } else {
      simulate_key_action();
      if ((static_cast<unsigned int>(when()) & static_cast<unsigned int>(FL_WHEN_CHANGED)) != 0U) {
        set_changed();
        Fl_Widget_Tracker wp(this);
        do_callback(FL_REASON_CHANGED);
        if (wp.deleted() != 0) {
          trigger_ret = 1;
        } else {
          set_changed();
          do_callback(FL_REASON_RELEASED);
        }
      } else if ((static_cast<unsigned int>(when()) & static_cast<unsigned int>(FL_WHEN_RELEASE)) != 0U) {
        set_changed();
        do_callback(FL_REASON_RELEASED);
      } else {
        // MISRA compliance: final else clause
      }
    }
    return trigger_ret;
  };

  switch (event) {
    case FL_ENTER: /* FALLTHROUGH */
    case FL_LEAVE:
      ret = 1;
      break;

    case FL_PUSH:
      if ((Fl::visible_focus() != 0) && (handle(FL_FOCUS) != 0)) {
        Fl::focus(this);
      }
      /* FALLTHROUGH */
      {
        signed char newval = 0;
        if (Fl::event_inside(this) != 0) {
          if (static_cast<unsigned char>(type()) == static_cast<unsigned char>(FL_RADIO_BUTTON)) {
            newval = 1;
          } else {
            newval = static_cast<signed char>((oldval == 0) ? 1 : 0);
          }
        } else {
          clear_changed();
          newval = oldval;
        }
        if (newval != value_) {
          value_ = newval;
          set_changed();
          if ((box() != FL_NO_BOX) && (fl_box(box()) == box())) {
            redraw();
          } else {
            redraw_label();
          }
          if ((static_cast<unsigned int>(when()) & static_cast<unsigned int>(FL_WHEN_CHANGED)) != 0U) {
            do_callback(FL_REASON_CHANGED);
          }
        }
      }
      ret = 1;
      break;

    case FL_DRAG: {
      signed char newval = 0;
      if (Fl::event_inside(this) != 0) {
        if (static_cast<unsigned char>(type()) == static_cast<unsigned char>(FL_RADIO_BUTTON)) {
          newval = 1;
        } else {
          newval = static_cast<signed char>((oldval == 0) ? 1 : 0);
        }
      } else {
        clear_changed();
        newval = oldval;
      }
      if (newval != value_) {
        value_ = newval;
        set_changed();
        if ((box() != FL_NO_BOX) && (fl_box(box()) == box())) {
          redraw();
        } else {
          redraw_label();
        }
        if ((static_cast<unsigned int>(when()) & static_cast<unsigned int>(FL_WHEN_CHANGED)) != 0U) {
          do_callback(FL_REASON_CHANGED);
        }
      }
      ret = 1;
      break;
    }

    case FL_RELEASE:
      if (value_ == oldval) {
        if ((static_cast<unsigned int>(when()) & static_cast<unsigned int>(FL_WHEN_NOT_CHANGED)) != 0U) {
          do_callback(FL_REASON_SELECTED);
        }
        ret = 1;
      } else {
        if (static_cast<unsigned char>(type()) == static_cast<unsigned char>(FL_RADIO_BUTTON)) {
          setonly();
          set_changed();
        } else if (static_cast<unsigned char>(type()) == static_cast<unsigned char>(FL_TOGGLE_BUTTON)) {
          oldval = value_;
          set_changed();
        } else {
          (void)value(oldval);
          set_changed();
          if ((static_cast<unsigned int>(when()) & static_cast<unsigned int>(FL_WHEN_CHANGED)) != 0U) {
            Fl_Widget_Tracker wp(this);
            do_callback(FL_REASON_CHANGED);
            if (wp.deleted() != 0) {
              ret = 1;
              break;
            }
          }
        }
        if ((static_cast<unsigned int>(when()) & static_cast<unsigned int>(FL_WHEN_RELEASE)) != 0U) {
          do_callback(FL_REASON_RELEASED);
        }
        ret = 1;
      }
      break;

    case FL_SHORTCUT: {
      const bool has_shortcut = (shortcut() != 0) ? (Fl::test_shortcut(shortcut()) != 0) : (test_shortcut() != 0);
      if (!has_shortcut) {
        ret = 0;
      } else {
        if ((Fl::visible_focus() != 0) && (handle(FL_FOCUS) != 0)) {
          Fl::focus(this);
        }
        ret = handle_keyboard_trigger();
      }
      break;
    }

    case FL_FOCUS:
    case FL_UNFOCUS:
      if (Fl::visible_focus() != 0) {
        if (Fl::box_bg(box()) == 0) {
          Fl_Window* const win = window();
          if (win != nullptr) {
            const int X = (x() > 0) ? (x() - 1) : 0;
            const int Y = (y() > 0) ? (y() - 1) : 0;
            win->damage(FL_DAMAGE_ALL, X, Y, w() + 2, h() + 2);
          }
        } else {
          if ((box() != FL_NO_BOX) && (fl_box(box()) == box())) {
            redraw();
          } else {
            redraw_label();
          }
        }
        ret = 1;
      } else {
        ret = 0;
      }
      break;

    case FL_KEYBOARD: {
      const auto state = static_cast<unsigned int>(Fl::event_state());
      constexpr unsigned int mask = static_cast<unsigned int>(FL_SHIFT) | static_cast<unsigned int>(FL_CTRL) | static_cast<unsigned int>(FL_ALT) | static_cast<unsigned int>(FL_META);
      if ((Fl::focus() == this) && (Fl::event_key() == ' ') && ((state & mask) == 0U)) {
        ret = handle_keyboard_trigger();
      } else {
        ret = 0;
      }
      break;
    }

    default:
      ret = 0;
      break;
  }
  
  return ret;
}

void Fl_Button::simulate_key_action()
{
  if (key_release_tracker != nullptr) {
    Fl::remove_timeout(key_release_timeout, key_release_tracker);
    key_release_timeout(key_release_tracker);
  }
  (void)value(1);
  redraw();
  key_release_tracker = new Fl_Widget_Tracker(this);
  Fl::add_timeout(0.15, key_release_timeout, key_release_tracker);
}

void Fl_Button::key_release_timeout(void *d)
{
  if (d != nullptr) {
    Fl_Widget_Tracker* const wt = static_cast<Fl_Widget_Tracker*>(d);
    if (wt == key_release_tracker) {
      key_release_tracker = nullptr;
    }
    Fl_Button* const btn = dynamic_cast<Fl_Button*>(wt->widget());
    if (btn != nullptr) {
      (void)btn->value(0);
      btn->redraw();
    }
    delete wt;
  }
}

/**
 The constructor creates the button using the given position, size, and label.

 The default box type is box(FL_UP_BOX).

 You can control how the button is drawn when ON by setting down_box().
 The default is FL_NO_BOX (0) which will select an appropriate box type
 using the normal (OFF) box type by using fl_down(box()).

 Derived classes may handle this differently.

 Calling `when()` will tell the button widget when to call the callback.

 Setting `FL_WHEN_RELEASE` will call the callback only if the button value
 changed. It's called during `FL_RELEASE` and `FL_KEYBOARD` events with
 `FL_REASON_RELEASED` set as the callback reason.

 Setting `FL_WHEN_CHANGED` will call the callback with `FL_REASON_CHANGED`
 every time the value of the button changes during `FL_DRAG`, `FL_RELEASE`,
 and `FL_KEYBOARD` events.

 Setting `FL_WHEN_NOT_CHANGED` will trigger a callback during `FL_RELEASE`
 events, even if the value of the button die *not* change. For radio buttons,
 this is also true during `FL_KEYBOARD` events.

 \param[in] X, Y, W, H position and size of the widget
 \param[in] L widget label, default is no label
 */
Fl_Button::Fl_Button(int X, int Y, int W, int H, const char *L)
: Fl_Widget(X, Y, W, H, L),
value_(0),
oldval(0),
down_box_(static_cast<uchar>(FL_NO_BOX)),
compact_(0)
{
  box(FL_UP_BOX);
  set_flag(SHORTCUT_LABEL);
}

/**
 The constructor creates the button using the given position, size, and label.

 The Button type() is set to FL_RADIO_BUTTON.

 \param[in] X, Y, W, H position and size of the widget
 \param[in] L widget label, default is no label
 */
Fl_Radio_Button::Fl_Radio_Button(int X, int Y, int W, int H, const char *L)
: Fl_Button(X, Y, W, H, L) {
  type(FL_RADIO_BUTTON);
}

/**
 The constructor creates the button using the given position, size, and label.

 The Button type() is set to FL_TOGGLE_BUTTON.

 \param[in] X, Y, W, H position and size of the widget
 \param[in] L widget label, default is no label
 */
Fl_Toggle_Button::Fl_Toggle_Button(int X, int Y, int W, int H, const char *L)
: Fl_Button(X, Y, W, H, L)
{
  type(FL_TOGGLE_BUTTON);
}

/**
 Decide if buttons should be rendered in compact mode.

 \image html compact_buttons_gtk.png "compact button keypad using GTK+ Scheme"
 \image latex compact_buttons_gtk.png "compact button keypad using GTK+ Scheme" width=4cm

 \image html compact_buttons_gleam.png "compact buttons in Gleam"
 \image latex compact_buttons_gleam.png "compact buttons in Gleam" width=4cm

 In compact mode, the button's surrounding border is altered to visually signal
 that multiple buttons are functionally linked together. To ensure the correct
 rendering of buttons in compact mode, all buttons must be part of the same
 group, positioned close to each other, and aligned with the edges of the
 group. Any button outlines not in contact with the parent group's outline
 will be displayed as separators.

 \param[in] v switch compact mode on (1) or off (0)
 */
void Fl_Button::compact(uchar v) { compact_ = v; }

void Fl_Button::shortcut(int s) {
  if (s != 0 && !ext_) alloc_ext();
  if (ext_) ext_->shortcut_ = s;
}

/// (for backwards compatibility)
void Fl_Button::shortcut(const char *s) {shortcut(fl_old_shortcut(s));}
