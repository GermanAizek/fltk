//
// Fl_NFC test for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_NFC.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <stdio.h>
#include "unittests.h"

static Fl_NFC_Manager *nfc = 0;
static Fl_Box *status_box = 0;

static void target_detected_cb(Fl_NFC_Target* target, void* data) {
  if (status_box) {
    status_box->copy_label("NFC Target Detected!");
    status_box->redraw();
  }
}

static void target_lost_cb(Fl_NFC_Target* target, void* data) {
  if (status_box) {
    status_box->copy_label("NFC Target Lost");
    status_box->redraw();
  }
}

static void start_cb(Fl_Widget* w, void*) {
  if (nfc && status_box) {
    if (nfc->start_target_detection()) {
      status_box->copy_label("NFC Detection Started...");
    } else {
      status_box->copy_label("NFC Not Supported / Failed to Start");
    }
    status_box->redraw();
  }
}

static void stop_cb(Fl_Widget* w, void*) {
  if (nfc && status_box) {
    nfc->stop_target_detection();
    status_box->copy_label("NFC Detection Stopped");
    status_box->redraw();
  }
}

static Fl_Widget* create_nfc_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  nfc = new Fl_NFC_Manager();
  nfc->target_detected_callback(target_detected_cb);
  nfc->target_lost_callback(target_lost_cb);
  
  status_box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 50, UT_TESTAREA_W - 20, UT_TESTAREA_H/2 - 50, "NFC Test Ready");
  status_box->box(FL_DOWN_BOX);
  status_box->color(FL_WHITE);
  status_box->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
  
  Fl_Button *start_btn = new Fl_Button(UT_TESTAREA_X + 10, UT_TESTAREA_Y + UT_TESTAREA_H/2 + 10, 150, 40, "Start Detection");
  start_btn->callback(start_cb);
  
  Fl_Button *stop_btn = new Fl_Button(UT_TESTAREA_X + 170, UT_TESTAREA_Y + UT_TESTAREA_H/2 + 10, 150, 40, "Stop Detection");
  stop_btn->callback(stop_cb);
  
  grp->end();
  
  return grp;
}

UnitTest nfc_test(UT_TEST_NFC, "NFC", create_nfc_test);
