//
// FLTK NFC Linux/X11 Driver
//
// Copyright 2026 by GermanAizek.
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

#ifndef Fl_X11_NFC_Driver_H
#define Fl_X11_NFC_Driver_H

#include <config.h>
#include "../../Fl_NFC_Driver.H"

class Fl_X11_NFC_Driver : public Fl_NFC_Driver {
public:
  Fl_X11_NFC_Driver(Fl_NFC_Manager* m);
  virtual ~Fl_X11_NFC_Driver();

  int start_target_detection() FL_OVERRIDE;
  void stop_target_detection() FL_OVERRIDE;

private:
#ifdef HAVE_PCSC_WINSCARD_H
  long context_;
#endif
  static void timer_cb(void*);
};

#endif // Fl_X11_NFC_Driver_H
