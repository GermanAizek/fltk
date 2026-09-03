//
// Fullscreen window support for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
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

#include <FL/Fl_Window.H>
#include "Fl_Window_Driver.H"

void Fl_Window::border(int b) {
  if (b) {
    if (border()) return;
    clear_flag(NOBORDER);
  } else {
    if (!border()) return;
    set_flag(NOBORDER);
  }
  pWindowDriver->use_border();
}

/* Note: The previous implementation toggled border(). With this new
   implementation this is not necessary. Additionally, if we do that,
   the application may lose focus when switching out of fullscreen
   mode with some window managers. Besides, the API does not say that
   the FLTK border state should be toggled; it only says that the
   borders should not be *visible*.
*/
void Fl_Window::fullscreen() {
  if (!is_resizable()) return;
  if (!maximize_active()) {
    Ext* e = ensure_ext();
    e->no_fullscreen_x = x();
    e->no_fullscreen_y = y();
    e->no_fullscreen_w = w();
    e->no_fullscreen_h = h();
  }
  if (shown() && !(flags() & Fl_Widget::FULLSCREEN)) {
    pWindowDriver->fullscreen_on();
  } else {
    set_flag(FULLSCREEN);
  }
}

/**
  Turns off any side effects of fullscreen() and does resize(X,Y,W,H).
*/
void Fl_Window::fullscreen_off(int X,int Y,int W,int H) {
  if (shown() && (flags() & Fl_Widget::FULLSCREEN)) {
    pWindowDriver->fullscreen_off(X, Y, W, H);
  } else {
    clear_flag(FULLSCREEN);
  }
  if (!maximize_active() && ext_)
    ext_->no_fullscreen_x = ext_->no_fullscreen_y = ext_->no_fullscreen_w = ext_->no_fullscreen_h = 0;
}

void Fl_Window::fullscreen_off() {
  int nx = ext_ ? ext_->no_fullscreen_x : 0;
  int ny = ext_ ? ext_->no_fullscreen_y : 0;
  int nw = ext_ ? ext_->no_fullscreen_w : 0;
  int nh = ext_ ? ext_->no_fullscreen_h : 0;
  if (!nx && !ny) {
    // Window was initially created fullscreen - default to current monitor
    nx = x();
    ny = y();
  }
  fullscreen_off(nx, ny, nw, nh);
}

/**
  Sets which screens should be used when this window is in fullscreen
  mode. The window will be resized to the top of the screen with index
  \p top, the bottom of the screen with index \p bottom, etc.

  If this method is never called, or if any argument is < 0, then the
  window will be resized to fill the screen it is currently on.

  Wayland compositors may support only partially or not at all multi-screen
  fullscreen windows.

  \see void Fl_Window::fullscreen()
  */
void Fl_Window::fullscreen_screens(int top, int bottom, int left, int right) {
  if ((top < 0) || (bottom < 0) || (left < 0) || (right < 0)) {
    if (ext_) {
      ext_->fullscreen_screen_top = -1;
      ext_->fullscreen_screen_bottom = -1;
      ext_->fullscreen_screen_left = -1;
      ext_->fullscreen_screen_right = -1;
    }
    pWindowDriver->fullscreen_screens(false);
  } else {
    Ext* e = ensure_ext();
    e->fullscreen_screen_top = top;
    e->fullscreen_screen_bottom = bottom;
    e->fullscreen_screen_left = left;
    e->fullscreen_screen_right = right;
    pWindowDriver->fullscreen_screens(true);
  }

  if (shown() && fullscreen_active())
    pWindowDriver->fullscreen_on();
}
