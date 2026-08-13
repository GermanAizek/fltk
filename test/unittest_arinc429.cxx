//
// Fl_ARINC429 test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_ARINC429.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"

static int callback_count = 0;
static uint32_t last_label = 0;
static uint32_t last_sdi = 0;
static uint32_t last_data = 0;
static uint32_t last_ssm = 0;

static void arinc_cb(Fl_ARINC429* arinc, void* d) {
  callback_count++;
  Fl_ARINC429::Word w = arinc->last_word();
  last_label = w.label;
  last_sdi = w.sdi;
  last_data = w.data;
  last_ssm = w.ssm;
}

static Fl_Widget* create_arinc429_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  Fl_Box *box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  box->box(FL_DOWN_BOX);
  box->color(FL_WHITE);
  box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
  
  Fl_ARINC429 arinc;
  arinc.arinc429_callback(arinc_cb);
  
  char buf[512];
  
  // Create a 32-bit ARINC 429 word
  // Label = 0x42, SDI = 0x01, Data = 0x12345, SSM = 0x03
  // Parity needs to be odd.
  // 0x42 (8) | 0x01 << 8 (2) | 0x12345 << 10 (19) | 0x03 << 29 (2)
  uint32_t raw_word = 0x42 | (0x01 << 8) | (0x12345 << 10) | (0x03 << 29);
  
  // Count ones
  int ones = 0;
  for(int i=0; i<31; i++) {
    if(raw_word & (1U << i)) ones++;
  }
  if((ones % 2) == 0) {
    // Parity bit 31
    raw_word |= (1U << 31);
  }
  
  arinc.feed_raw_word(raw_word);
  
  if (callback_count == 1 && last_label == 0x42 && last_sdi == 0x01 && last_data == 0x12345 && last_ssm == 0x03) {
    snprintf(buf, sizeof(buf), "ARINC 429 parsing OK.\nLabel: 0x%X\nSDI: 0x%X\nData: 0x%X\nSSM: 0x%X", 
             last_label, last_sdi, last_data, last_ssm);
  } else {
    snprintf(buf, sizeof(buf), "ARINC 429 parsing FAILED.\nLabel: 0x%X (expected 0x42)\nSDI: 0x%X (expected 0x1)\nData: 0x%X (expected 0x12345)\nSSM: 0x%X (expected 0x3)", 
             last_label, last_sdi, last_data, last_ssm);
  }
  
  // Test invalid parity
  uint32_t invalid_word = raw_word ^ (1U << 31); // flip parity
  int pre_count = callback_count;
  arinc.feed_raw_word(invalid_word);
  
  if (callback_count == pre_count) {
    strcat(buf, "\n\nInvalid parity word correctly ignored.");
  } else {
    strcat(buf, "\n\nFAILED: Invalid parity word triggered callback.");
  }
  
  box->copy_label(buf);
  grp->end();
  
  return grp;
}

UnitTest arinc429_test(UT_TEST_ARINC429, "ARINC 429", create_arinc429_test);
