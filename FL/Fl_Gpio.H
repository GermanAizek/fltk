//
// GPIO class for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by Herman Semenoff (GermanAizek)
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

#ifndef Fl_Gpio_H
#define Fl_Gpio_H

#include "Fl_Export.H"
#include "Fl.H"

/**
  Cross-platform GPIO device wrapper.
  
  This class provides a uniform interface to read and write GPIO pins.
  On Linux, it utilizes the standard sysfs interface (`/sys/class/gpio/`)
  and integrates with FLTK's event loop via `Fl::add_fd` to receive
  interrupts when edges change (for input pins).
  
  On other platforms (Windows, macOS), the methods act as stubs and return
  error codes (-1) when operations fail, allowing cross-platform
  compilation without conditional logic in user code.
*/
class FL_EXPORT Fl_Gpio {
  int pin_;
  int fd_;
  int dir_;
  int edge_;
  void (*cb_)(Fl_Gpio*, void*);
  void* data_;

  int export_pin();
  int unexport_pin();
  int set_edge_sysfs(int edge_mode);
  int set_dir_sysfs(int dir_mode);
  
  static void fd_callback(int fd, void* data);

public:
  enum Direction {
    IN = 0,
    OUT = 1
  };
  
  enum Edge {
    NONE = 0,
    RISING = 1,
    FALLING = 2,
    BOTH = 3
  };

  /**
    Initializes a GPIO instance for the specified pin number.
    The pin is exported if available on the system.
  */
  Fl_Gpio(int pin);
  
  /**
    Cleans up the GPIO instance, unexporting the pin.
  */
  ~Fl_Gpio();

  /** Returns the hardware pin number this object controls. */
  int pin() const { return pin_; }

  /** 
    Sets the direction of the pin.
    \param d Direction (Fl_Gpio::IN or Fl_Gpio::OUT)
    \return 0 on success, or -1 on error
  */
  int direction(Direction d);
  
  /** Returns the current configured direction of the pin. */
  Direction direction() const { return (Direction)dir_; }

  /**
    Sets the logic value of an OUT pin.
    \param v 0 for LOW, 1 for HIGH
    \return 0 on success, or -1 on error
  */
  int value(int v);
  
  /**
    Reads the current logic value of an IN pin.
    \return 0 for LOW, 1 for HIGH, or -1 on error
  */
  int value() const;

  /**
    Sets the edge detection mode for an IN pin.
    \param e Edge detection mode
    \return 0 on success, or -1 on error
  */
  int edge(Edge e);
  
  /** Returns the current edge detection mode. */
  Edge edge() const { return (Edge)edge_; }

  /**
    Registers a callback function to be called when the configured edge is detected.
    This integrates with FLTK's main event loop.
    \param cb Callback function
    \param data Optional user data passed to the callback
  */
  void callback(void (*cb)(Fl_Gpio*, void*), void* data = 0);
  
  /** Invokes the registered callback. */
  void do_callback();
};

#endif
