//
// FLTK NFC (Near Field Communication) driver interface
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

#ifndef Fl_NFC_Driver_H
#define Fl_NFC_Driver_H

#include <FL/Fl_NFC.H>

class Fl_NFC_Driver {
protected:
  Fl_NFC_Manager* manager_;

public:
  Fl_NFC_Driver(Fl_NFC_Manager* m) : manager_(m) {}
  virtual ~Fl_NFC_Driver() {}

  virtual int start_target_detection() = 0;
  virtual void stop_target_detection() = 0;
  
  // Factory method implemented in src/Fl_NFC.cxx based on platform
  static Fl_NFC_Driver* create(Fl_NFC_Manager* m);
};

#endif // Fl_NFC_Driver_H
