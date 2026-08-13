//
// Fl_CRSF test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_CRSF.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"
#include <stdio.h>

static Fl_Box *crsf_status_box = 0;

static void crsf_cb(Fl_CRSF* crsf, void* data) {
  if (crsf_status_box) {
    char buf[256];
    snprintf(buf, sizeof(buf), "CRSF Frame Received!\nCH1: %d\nCH2: %d\nCH3: %d\nCH4: %d", 
             crsf->channel(1), crsf->channel(2), crsf->channel(3), crsf->channel(4));
    crsf_status_box->copy_label(buf);
    crsf_status_box->redraw();
  }
}

static Fl_Widget* create_crsf_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  crsf_status_box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  crsf_status_box->box(FL_DOWN_BOX);
  crsf_status_box->color(FL_WHITE);
  crsf_status_box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
  crsf_status_box->copy_label("Waiting for CRSF frame...");
  
  static Fl_CRSF crsf;
  crsf.crsf_callback(crsf_cb, nullptr);
  
  // Simulate a CRSF packet (Type 0x16 RC Channels)
  // 1500us -> 992 raw value (0x3E0) for all channels
  uint8_t packet[] = {
    0xC8, // Sync
    24,   // Length (Type + Payload + CRC)
    0x16, // Type
    // Payload (22 bytes, 16x 11-bit channels set to 992)
    0xE0, 0x03, 0x1F, 0x2B, 0x7C, 0xE0, 0x03, 0x1F, 
    0x2B, 0x7C, 0xE0, 0x03, 0x1F, 0x2B, 0x7C, 0xE0, 
    0x03, 0x1F, 0x2B, 0x7C, 0xE0, 0x03,
    0x00  // CRC8 (will calculate below)
  };
  
  // Calculate CRC8 properly for the mock payload so it passes validation
  uint8_t crc = 0;
  for (int i = 2; i < 2+23; i++) {
    crc ^= packet[i];
    for (int j = 0; j < 8; j++) {
      if ((crc & 0x80) != 0) crc = (uint8_t)((crc << 1) ^ 0xD5);
      else crc <<= 1;
    }
  }
  packet[25] = crc; // Last byte is CRC
  
  // Feed the packet to trigger the callback
  for (int i = 0; i < 26; i++) {
    crsf.feed_byte(packet[i]);
  }
  
  grp->end();
  
  return grp;
}

UnitTest crsf_test(UT_TEST_CRSF, "CRSF Protocol", create_crsf_test);
