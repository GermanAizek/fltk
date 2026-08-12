//
// Fl_Serial_Port test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_Serial_Port.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"

static Fl_Widget* create_serial_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  Fl_Box *box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  box->box(FL_DOWN_BOX);
  box->color(FL_WHITE);
  box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
  
  Fl_Serial_Port port;
  // Test methods on a non-existent port
  int res = port.open("FLTK_TEST_DUMMY_PORT");
  
  char buf[256];
  if (res == -1) {
    snprintf(buf, sizeof(buf), "Serial port successfully failed to open dummy port.\n(Expected behavior)\nis_open: %d", port.is_open());
  } else {
    snprintf(buf, sizeof(buf), "WARNING: Opened dummy port successfully? This is unexpected.");
  }
  
  // Test configurations
  port.set_baud_rate(9600);
  port.set_data_bits(Fl_Serial_Port::DATA_8);
  port.set_parity(Fl_Serial_Port::PARITY_NONE);
  port.set_stop_bits(Fl_Serial_Port::STOP_1);
  
  box->copy_label(buf);
  
  grp->end();
  
  return grp;
}

UnitTest serial_test(UT_TEST_SERIAL, "Serial Port", create_serial_test);
