//
// Fl_ActiveX test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
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

#include <FL/Fl_ActiveX.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include "unittests.h"
#include <string.h>

static Fl_ActiveX *ax = 0;
static Fl_Text_Buffer *textbuf = 0;
static Fl_Text_Display *textdisp = 0;

static void test_cb(Fl_Widget *w, void *data) {
  if (!ax) return;
  textbuf->text("");
  
  if (ax->set_control("Word.Application")) {
    textbuf->append("Successfully set ActiveX control to Word.Application\n");
    char buf[256];
    snprintf(buf, sizeof(buf), "Current control: %s\n", ax->control() ? ax->control() : "NULL");
    textbuf->append(buf);
  } else {
    textbuf->append("Failed to set ActiveX control.\n");
  }
}

Fl_Widget *create_activex_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();
  
  ax = new Fl_ActiveX(UT_TESTAREA_X + 170, UT_TESTAREA_Y + 10, 150, 100, "ActiveX Control");
  
  Fl_Button *btn = new Fl_Button(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, 150, 30, "Test ActiveX");
  btn->callback(test_cb);
  
  textbuf = new Fl_Text_Buffer();
  textdisp = new Fl_Text_Display(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 120, UT_TESTAREA_W - 20, UT_TESTAREA_H - 140);
  textdisp->buffer(textbuf);
  textbuf->append("Click 'Test ActiveX' to initialize control.\n");
  
  g->end();
  return g;
}

UnitTest activex_test(UT_TEST_ACTIVEX, "ActiveX", create_activex_test);
