//
// Fl_SBUS test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_SBUS.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"
#include <stdio.h>

static Fl_Box *sbus_status_box = 0;

static void sbus_cb(Fl_SBUS* sbus, void* data) {
  if (sbus_status_box) {
    char buf[256];
    snprintf(buf, sizeof(buf), "SBUS Frame Received!\nCH1: %d\nCH2: %d\nCH16: %d\nFlags: FS=%d FL=%d", 
             sbus->channel(1), sbus->channel(2), sbus->channel(16), 
             sbus->failsafe() ? 1 : 0, sbus->frame_lost() ? 1 : 0);
    sbus_status_box->copy_label(buf);
    sbus_status_box->redraw();
  }
}

static Fl_Widget* create_sbus_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  sbus_status_box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  sbus_status_box->box(FL_DOWN_BOX);
  sbus_status_box->color(FL_WHITE);
  sbus_status_box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
  sbus_status_box->copy_label("Waiting for SBUS frame...");
  
  static Fl_SBUS sbus;
  sbus.sbus_callback(sbus_cb, nullptr);
  
  // Simulate an SBUS packet (1024 for all channels)
  uint8_t packet[25] = {0};
  packet[0] = 0x0F;
  // Encode 1024 (0x400) into 11-bit channels
  // CH1 = 1024 -> 0x400
  packet[1] = 0x00;
  packet[2] = 0x08;
  // This is just a dummy packet to test parsing trigger
  packet[23] = 0x08; // Failsafe active
  packet[24] = 0x00; // Footer
  
  // Feed the packet to trigger the callback
  for (int i = 0; i < 25; i++) {
    sbus.feed_byte(packet[i]);
  }
  
  grp->end();
  
  return grp;
}

UnitTest sbus_test(UT_TEST_SBUS, "SBUS Protocol", create_sbus_test);
