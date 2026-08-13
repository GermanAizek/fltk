//
// Fl_MSP test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_MSP.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"
#include <stdio.h>

static Fl_Box *msp_status_box = 0;

static void msp_cb(Fl_MSP* msp, uint8_t cmd, const uint8_t* payload, uint8_t size, void* data) {
  if (msp_status_box) {
    char buf[256];
    if (cmd == 105 && size >= 8) { // MSP_RC
      uint16_t ch1 = payload[0] | (payload[1] << 8);
      uint16_t ch2 = payload[2] | (payload[3] << 8);
      uint16_t ch3 = payload[4] | (payload[5] << 8);
      uint16_t ch4 = payload[6] | (payload[7] << 8);
      
      snprintf(buf, sizeof(buf), "MSP_RC (105) Received!\nCH1: %d\nCH2: %d\nCH3: %d\nCH4: %d", 
               ch1, ch2, ch3, ch4);
    } else {
      snprintf(buf, sizeof(buf), "MSP CMD %d Received!\nPayload Size: %d bytes", cmd, size);
    }
    
    msp_status_box->copy_label(buf);
    msp_status_box->redraw();
  }
}

static Fl_Widget* create_msp_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  msp_status_box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  msp_status_box->box(FL_DOWN_BOX);
  msp_status_box->color(FL_WHITE);
  msp_status_box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
  msp_status_box->copy_label("Waiting for MSP frame...");
  
  static Fl_MSP msp;
  msp.msp_callback(msp_cb, nullptr);
  
  // Simulate an MSP_RC packet (cmd 105)
  // Payload is 16 channels, 2 bytes each = 32 bytes
  // Frame: $ M > size cmd [payload] checksum
  uint8_t packet[38] = {0};
  packet[0] = '$';
  packet[1] = 'M';
  packet[2] = '>';
  packet[3] = 32; // Size
  packet[4] = 105; // CMD: MSP_RC
  
  // Payload (16 channels set to 1500 -> 0x05DC)
  for (int i = 0; i < 16; i++) {
    packet[5 + i*2] = 0xDC; // LSB
    packet[6 + i*2] = 0x05; // MSB
  }
  
  // Calculate checksum
  uint8_t checksum = packet[3] ^ packet[4];
  for (int i = 0; i < 32; i++) {
    checksum ^= packet[5 + i];
  }
  packet[37] = checksum;
  
  // Feed the packet to trigger the callback
  for (int i = 0; i < 38; i++) {
    msp.feed_byte(packet[i]);
  }
  
  grp->end();
  
  return grp;
}

UnitTest msp_test(UT_TEST_MSP, "MSP Protocol", create_msp_test);
