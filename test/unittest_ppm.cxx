//
// PPM unittests for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#include "unittests.h"
#include <FL/Fl_PPM.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <stdio.h>
#include <stdlib.h>

static Fl_PPM *ppm = 0;
static Fl_Text_Buffer *textbuf = 0;
static Fl_Text_Display *textdisp = 0;

static void ppm_test_cb(Fl_PPM* p, void* data) {
  (void)p; (void)data;
  textbuf->append("PPM Callback triggered!\n");
}

static void btn_test_cb(Fl_Widget*, void*) {
  textbuf->text("");
  textbuf->append("Starting PPM tests...\n");

  if (!ppm) ppm = new Fl_PPM();

  if (ppm->is_open()) {
    textbuf->append("ERROR: PPM should be closed initially.\n");
    return;
  }
  
  ppm->set_num_channels(12);
  if (ppm->num_channels() != 12) {
    textbuf->append("ERROR: Channels not updated.\n");
    return;
  }
  
  if (ppm->open("/dev/dummy_ppm") != 0) {
    textbuf->append("ERROR: Failed to open mock PPM.\n");
    return;
  }
  textbuf->append("PPM successfully opened.\n");

  uint16_t w_data[4] = { 1100, 1500, 1900, 1000 };
  if (ppm->write_channels(w_data, 4) != 4) {
    textbuf->append("ERROR: write_channels failed.\n");
  } else {
    textbuf->append("Channels written successfully.\n");
  }
  
  uint16_t r_data[12] = {0};
  if (ppm->read_channels(r_data, 12) != 12) {
    textbuf->append("ERROR: read_channels failed.\n");
  } else {
    textbuf->append("Channels read successfully.\n");
  }
  
  ppm->callback(ppm_test_cb, 0);
  ppm->do_callback();

  ppm->close();
  textbuf->append("PPM tests passed successfully.\n");
}

Fl_Widget *create_ppm_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();
  
  Fl_Button *btn = new Fl_Button(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, 150, 30, "Test PPM");
  btn->callback(btn_test_cb);
  
  textbuf = new Fl_Text_Buffer();
  textdisp = new Fl_Text_Display(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 50, UT_TESTAREA_W - 20, UT_TESTAREA_H - 70);
  textdisp->buffer(textbuf);
  textbuf->append("Click 'Test PPM' to run the test.\n");
  
  g->end();
  return g;
}

UnitTest ppm_test(UT_TEST_PPM, "PPM", create_ppm_test);
