//
// Keyboard state routines for the Fast Light Tool Kit (FLTK).
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

#if !defined(FL_DOXYGEN)

// Return the current state of a key.  This is the X version.  I identify
// keys (mostly) by the X keysym.  So this turns the keysym into a keycode
// and looks it up in the X key bit vector, which Fl_x.cxx keeps track of.

#include <FL/Fl.H>
#include "drivers/X11/Fl_X11_Screen_Driver.H"
#include <FL/platform.H> // for fl_display

extern char fl_key_vector[32]; // in Fl_x.cxx

int Fl_X11_Screen_Driver::event_key(const int k) {
  if (k > FL_Button && k <= FL_Button + 8) {
    const auto shift = static_cast<unsigned int>(k - FL_Button);
    return Fl::event_state(static_cast<int>(8U << shift));
  }

  int i = 0;
#  ifdef __sgi
  // get some missing PC keyboard keys:
  if (k == FL_Meta_L) i = 147;
  else if (k == FL_Meta_R) i = 148;
  else if (k == FL_Menu) i = 149;
  else
#  endif
    i = static_cast<int>(XKeysymToKeycode(fl_display, static_cast<KeySym>(k)));

  if (i == 0) return 0;

  const auto u_i = static_cast<unsigned int>(i);
  const auto bit_mask = static_cast<unsigned char>(1U << (u_i % 8U));
  const auto byte_val = static_cast<unsigned char>(fl_key_vector[u_i / 8U]);

  return (byte_val & bit_mask) != 0U ? 1 : 0;
}

int Fl_X11_Screen_Driver::get_key(const int k) {
  fl_open_display();
  XQueryKeymap(fl_display, fl_key_vector);
  return event_key(k);
}

#endif // FL_DOXYGEN