//
// Fl_XBUS test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_XBUS.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"
#include <stdio.h>

static Fl_Box *xbus_status_box = 0;

static void xbus_cb(Fl_XBUS* xbus, void* data) {
  if (xbus_status_box) {
    char buf[256];
    snprintf(buf, sizeof(buf), "XBUS Frame Received!\nCH1: %d\nCH2: %d\nCH3: %d\nCH4: %d", 
             xbus->channel(1), xbus->channel(2), xbus->channel(3), xbus->channel(4));
    xbus_status_box->copy_label(buf);
    xbus_status_box->redraw();
  }
}

static Fl_Widget* create_xbus_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  xbus_status_box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  xbus_status_box->box(FL_DOWN_BOX);
  xbus_status_box->color(FL_WHITE);
  xbus_status_box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
  xbus_status_box->copy_label("Waiting for XBUS frame...");
  
  static Fl_XBUS xbus;
  xbus.xbus_callback(xbus_cb, nullptr);
  
  // Simulate an XBUS (Mode B / SRXL) packet
  // Header is 0xA1, 16 channels, big-endian format.
  // We'll set the channels to 1024 (0x0400).
  uint8_t packet[35] = {0};
  packet[0] = 0xA1; // Header
  
  // 16 channels
  for (int i = 0; i < 16; i++) {
    packet[1 + i*2] = 0x04; // MSB
    packet[2 + i*2] = 0x00; // LSB
  }
  
  // Calculate CRC16-CCITT for the payload
  uint16_t crc = 0x0000;
  for (int i = 0; i < 33; i++) {
    crc ^= (packet[i] << 8);
    for (int j = 0; j < 8; j++) {
      if ((crc & 0x8000) > 0) {
        crc = (crc << 1) ^ 0x1021;
      } else {
        crc = (crc << 1);
      }
    }
  }
  
  packet[33] = (crc >> 8) & 0xFF; // CRC MSB
  packet[34] = crc & 0xFF;        // CRC LSB
  
  // Feed the packet to trigger the callback
  for (int i = 0; i < 35; i++) {
    xbus.feed_byte(packet[i]);
  }
  
  grp->end();
  
  return grp;
}

UnitTest xbus_test(UT_TEST_XBUS, "XBUS Protocol", create_xbus_test);
