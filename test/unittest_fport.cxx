//
// Fl_FPort test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_FPort.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"
#include <stdio.h>

static Fl_Box *fport_status_box = 0;

static void fport_cb(Fl_FPort* fport, void* data) {
  if (fport_status_box) {
    char buf[256];
    snprintf(buf, sizeof(buf), "FPort Frame Received!\nRSSI: %d\nFlags: 0x%02X\nCH1: %d\nCH2: %d\nCH3: %d\nCH4: %d", 
             fport->rssi(), fport->flags(),
             fport->channel(1), fport->channel(2), fport->channel(3), fport->channel(4));
    fport_status_box->copy_label(buf);
    fport_status_box->redraw();
  }
}

static Fl_Widget* create_fport_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  fport_status_box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  fport_status_box->box(FL_DOWN_BOX);
  fport_status_box->color(FL_WHITE);
  fport_status_box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
  fport_status_box->copy_label("Waiting for FPort frame...");
  
  static Fl_FPort fport;
  fport.fport_callback(fport_cb, nullptr);
  
  // Simulate an FPort Control packet (Type 0x00)
  // Payload has 22 bytes SBUS channels, 1 byte Flags, 1 byte RSSI.
  // We'll set the channels to 1024.
  uint8_t sbus[22] = {0};
  uint16_t ch_val = 1024;
  sbus[0]  = (uint8_t)(ch_val & 0xFF);
  sbus[1]  = (uint8_t)((ch_val & 0xFF) >> 3 | (ch_val & 0xFF) << 5);
  sbus[2]  = (uint8_t)((ch_val & 0xFF) >> 6 | (ch_val & 0xFF) << 2 | (ch_val & 0xFF) << 10);
  sbus[3]  = (uint8_t)((ch_val & 0xFF) >> 1 | (ch_val & 0xFF) << 7);
  sbus[4]  = (uint8_t)((ch_val & 0xFF) >> 4 | (ch_val & 0xFF) << 4);
  sbus[5]  = (uint8_t)((ch_val & 0xFF) >> 7 | (ch_val & 0xFF) << 1 | (ch_val & 0xFF) << 9);
  sbus[6]  = (uint8_t)((ch_val & 0xFF) >> 2 | (ch_val & 0xFF) << 6);
  sbus[7]  = (uint8_t)((ch_val & 0xFF) >> 5 | (ch_val & 0xFF) << 3);
  sbus[8]  = (uint8_t)(ch_val & 0xFF);
  sbus[9]  = (uint8_t)((ch_val & 0xFF) >> 3 | (ch_val & 0xFF) << 5);
  sbus[10] = (uint8_t)((ch_val & 0xFF) >> 6 | (ch_val & 0xFF) << 2 | (ch_val & 0xFF) << 10);
  sbus[11] = (uint8_t)((ch_val & 0xFF) >> 1 | (ch_val & 0xFF) << 7);
  sbus[12] = (uint8_t)((ch_val & 0xFF) >> 4 | (ch_val & 0xFF) << 4);
  sbus[13] = (uint8_t)((ch_val & 0xFF) >> 7 | (ch_val & 0xFF) << 1 | (ch_val & 0xFF) << 9);
  sbus[14] = (uint8_t)((ch_val & 0xFF) >> 2 | (ch_val & 0xFF) << 6);
  sbus[15] = (uint8_t)((ch_val & 0xFF) >> 5 | (ch_val & 0xFF) << 3);
  sbus[16] = (uint8_t)(ch_val & 0xFF);
  sbus[17] = (uint8_t)((ch_val & 0xFF) >> 3 | (ch_val & 0xFF) << 5);
  sbus[18] = (uint8_t)((ch_val & 0xFF) >> 6 | (ch_val & 0xFF) << 2 | (ch_val & 0xFF) << 10);
  sbus[19] = (uint8_t)((ch_val & 0xFF) >> 1 | (ch_val & 0xFF) << 7);
  sbus[20] = (uint8_t)((ch_val & 0xFF) >> 4 | (ch_val & 0xFF) << 4);
  sbus[21] = (uint8_t)((ch_val & 0xFF) >> 7 | (ch_val & 0xFF) << 1 | (ch_val & 0xFF) << 9);
  
  uint8_t packet[30] = {0};
  packet[0] = 0x7E;
  packet[1] = 0x19; // Length (25 bytes for RC: Type + 22 SBUS + 1 Flag + 1 RSSI)
  packet[2] = 0x00; // Type
  for (int i = 0; i < 22; i++) {
    packet[3 + i] = sbus[i];
  }
  packet[25] = 0x00; // Flags
  packet[26] = 99;   // RSSI
  
  // Calculate FrSky checksum over length + type + payload
  uint16_t sum = packet[1] + packet[2];
  for (int i = 0; i < 24; i++) {
    sum += packet[3 + i];
  }
  sum += sum >> 8;
  sum &= 0xFF;
  packet[27] = 0xFF - sum;
  
  // Feed the packet to trigger the callback
  for (int i = 0; i < 28; i++) {
    fport.feed_byte(packet[i]);
  }
  
  grp->end();
  
  return grp;
}

UnitTest fport_test(UT_TEST_FPORT, "FPort Protocol", create_fport_test);
