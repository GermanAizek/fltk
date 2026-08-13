//
// Fl_AFDX test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_AFDX.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"

static int callback_count = 0;
static uint16_t last_vl = 0;
static uint8_t last_seq = 0;

static void afdx_cb(Fl_AFDX* afdx, void* d) {
  callback_count++;
  Fl_AFDX::FrameInfo f = afdx->last_frame();
  last_vl = f.virtual_link;
  last_seq = f.seq_num;
}

static Fl_Widget* create_afdx_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  Fl_Box *box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  box->box(FL_DOWN_BOX);
  box->color(FL_WHITE);
  box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
  
  Fl_AFDX afdx;
  afdx.afdx_callback(afdx_cb);
  
  char buf[512];
  
  // Construct a dummy AFDX frame (just enough to parse Dest MAC and Seq)
  // Length needs to be at least 43 bytes
  uint8_t frame[50];
  memset(frame, 0, sizeof(frame));
  
  // Dest MAC: 03:00:00:00:12:34 (VL = 0x1234)
  frame[0] = 0x03;
  frame[1] = 0x00;
  frame[2] = 0x00;
  frame[3] = 0x00;
  frame[4] = 0x12; // VL High
  frame[5] = 0x34; // VL Low
  
  // Sequence Number at the end (payload size 50 - 15 = 35)
  frame[49] = 0x42; // Sequence Number = 66
  
  afdx.feed_raw_frame(frame, sizeof(frame));
  
  if (callback_count == 1 && last_vl == 0x1234 && last_seq == 0x42) {
    snprintf(buf, sizeof(buf), "AFDX (ARINC 664) parsing OK.\nVirtual Link ID: 0x%X\nSequence Number: 0x%X", 
             last_vl, last_seq);
  } else {
    snprintf(buf, sizeof(buf), "AFDX parsing FAILED.\nVL: 0x%X (expected 0x1234)\nSEQ: 0x%X (expected 0x42)", 
             last_vl, last_seq);
  }
  
  // Test invalid MAC format (not AFDX)
  frame[0] = 0xFF; // Broadcast MAC
  int pre_count = callback_count;
  afdx.feed_raw_frame(frame, sizeof(frame));
  
  if (callback_count == pre_count) {
    strcat(buf, "\n\nNon-AFDX frame correctly ignored.");
  } else {
    strcat(buf, "\n\nFAILED: Non-AFDX frame triggered callback.");
  }
  
  box->copy_label(buf);
  grp->end();
  
  return grp;
}

UnitTest afdx_test(UT_TEST_AFDX, "AFDX", create_afdx_test);
