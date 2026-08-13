//
// Fl_IBUS test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_IBUS.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"
#include <stdio.h>

static Fl_Box *ibus_status_box = 0;

static void ibus_cb(Fl_IBUS* ibus, void* data) {
  if (ibus_status_box) {
    char buf[256];
    snprintf(buf, sizeof(buf), "IBUS Frame Received!\nCH1: %d\nCH2: %d\nCH3: %d\nCH4: %d", 
             ibus->channel(1), ibus->channel(2), ibus->channel(3), ibus->channel(4));
    ibus_status_box->copy_label(buf);
    ibus_status_box->redraw();
  }
}

static Fl_Widget* create_ibus_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  ibus_status_box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  ibus_status_box->box(FL_DOWN_BOX);
  ibus_status_box->color(FL_WHITE);
  ibus_status_box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
  ibus_status_box->copy_label("Waiting for IBUS frame...");
  
  static Fl_IBUS ibus;
  ibus.ibus_callback(ibus_cb, nullptr);
  
  // Simulate an IBUS packet
  // A typical channel value is 1500 (0x05DC).
  // Little-endian: 0xDC, 0x05
  uint8_t packet[32] = {0};
  packet[0] = 0x20; // Length
  packet[1] = 0x40; // Type
  
  // 14 channels
  for (int i = 0; i < 14; i++) {
    packet[2 + i*2] = 0xDC; // LSB
    packet[3 + i*2] = 0x05; // MSB
  }
  
  // Calculate checksum: 0xFFFF - sum(bytes 0..29)
  uint16_t checksum = 0xFFFF;
  for (int i = 0; i < 30; i++) {
    checksum -= packet[i];
  }
  
  packet[30] = checksum & 0xFF;         // Checksum LSB
  packet[31] = (checksum >> 8) & 0xFF;  // Checksum MSB
  
  // Feed the packet to trigger the callback
  for (int i = 0; i < 32; i++) {
    ibus.feed_byte(packet[i]);
  }
  
  grp->end();
  
  return grp;
}

UnitTest ibus_test(UT_TEST_IBUS, "IBUS Protocol", create_ibus_test);
