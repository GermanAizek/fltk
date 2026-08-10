//
// Definition of Android system driver for the Fast Light Tool Kit (FLTK).
//
// Copyright 2010-2020 by Bill Spitzak and others.
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

/**
 \file Fl_Android_System_Driver.H
 \brief Definition of Android system driver.
 */

#ifndef FL_ANDROID_SYSTEM_DRIVER_H
#define FL_ANDROID_SYSTEM_DRIVER_H

#include <string.h>
#include <FL/fl_string_functions.h>
#include "../Posix/Fl_Posix_System_Driver.H"
#include <stdarg.h>

/*
 Move everything here that manages the system interface.

 There is exactly one system driver.

 - filename and pathname management
 - directory and file access
 - system time and system timer
 - multithreading
 */

class Fl_Android_System_Driver : public Fl_Posix_System_Driver
{
public:
  double wait(double time_to_wait) FL_OVERRIDE;
  int ready() FL_OVERRIDE;
  virtual char *strdup(const char *s) FL_OVERRIDE {return ::strdup(s);}
};

#endif // FL_ANDROID_SYSTEM_DRIVER_H
