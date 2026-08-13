//
// Fl_SUMD test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_SUMD.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"
#include <stdio.h>

static Fl_Box *sumd_status_box = 0;

static void sumd_cb(Fl_SUMD* sumd, void* data) {
  if (sumd_status_box) {
    char buf[256];
    snprintf(buf, sizeof(buf), "SUMD Frame Received!\nChannels: %d\nStatus: 0x%02X\nCH1: %d\nCH2: %d\nCH3: %d\nCH4: %d", 
             sumd->channel_count(), sumd->status(),
             sumd->channel(1), sumd->channel(2), sumd->channel(3), sumd->channel(4));
    sumd_status_box->copy_label(buf);
    sumd_status_box->redraw();
  }
}

static Fl_Widget* create_sumd_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  sumd_status_box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  sumd_status_box->box(FL_DOWN_BOX);
  sumd_status_box->color(FL_WHITE);
  sumd_status_box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
  sumd_status_box->copy_label("Waiting for SUMD frame...");
  
  static Fl_SUMD sumd;
  sumd.sumd_callback(sumd_cb, nullptr);
  
  // Simulate an SUMD packet (16 channels)
  // Header: 0xA8, Status: 0x01 (Valid), Count: 16 (0x10)
  uint8_t packet[37] = {0};
  packet[0] = 0xA8;
  packet[1] = 0x01;
  packet[2] = 16;
  
  // 16 channels, set to 4096 (0x1000)
  for (int i = 0; i < 16; i++) {
    packet[3 + i*2] = 0x10; // MSB
    packet[4 + i*2] = 0x00; // LSB
  }
  
  // Calculate CRC16-CCITT for the payload
  uint16_t crc = 0x0000;
  for (int i = 0; i < 35; i++) {
    crc ^= (packet[i] << 8);
    for (int j = 0; j < 8; j++) {
      if ((crc & 0x8000) > 0) {
        crc = (crc << 1) ^ 0x1021;
      } else {
        crc = (crc << 1);
      }
    }
  }
  
  packet[35] = (crc >> 8) & 0xFF; // CRC MSB
  packet[36] = crc & 0xFF;        // CRC LSB
  
  // Feed the packet to trigger the callback
  for (int i = 0; i < 37; i++) {
    sumd.feed_byte(packet[i]);
  }
  
  grp->end();
  
  return grp;
}

UnitTest sumd_test(UT_TEST_SUMD, "SUMD Protocol", create_sumd_test);
