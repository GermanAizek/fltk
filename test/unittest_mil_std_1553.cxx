//
// Fl_MIL_STD_1553 test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_MIL_STD_1553.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"

static int callback_count = 0;
static uint16_t last_data = 0;
static Fl_MIL_STD_1553::SyncType last_sync = Fl_MIL_STD_1553::SYNC_DATA;

static void mil_cb(Fl_MIL_STD_1553* mil, void* d) {
  callback_count++;
  Fl_MIL_STD_1553::Word w = mil->last_word();
  last_data = w.data;
  last_sync = w.sync;
}

static Fl_Widget* create_mil1553_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  Fl_Box *box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  box->box(FL_DOWN_BOX);
  box->color(FL_WHITE);
  box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
  
  Fl_MIL_STD_1553 mil;
  mil.mil_std_1553_callback(mil_cb);
  
  char buf[512];
  
  // Create a 24-bit representation of a MIL-STD-1553 word
  // Sync = 0 (Command/Status), Data = 0x55AA
  // 16-bit data + 1 parity bit = 17 bits for parity check
  // Parity needs to be odd.
  uint32_t raw_word = 0x55AA | (0U << 17);
  
  // Count ones in lower 16 bits
  int ones = 0;
  for(int i=0; i<16; i++) {
    if(raw_word & (1U << i)) ones++;
  }
  if((ones % 2) == 0) {
    // Set parity bit (bit 16)
    raw_word |= (1U << 16);
  }
  
  mil.feed_raw_word(raw_word);
  
  if (callback_count == 1 && last_data == 0x55AA && last_sync == Fl_MIL_STD_1553::SYNC_COMMAND_STATUS) {
    snprintf(buf, sizeof(buf), "MIL-STD-1553 parsing OK.\nData: 0x%X\nSync Type: %s", 
             last_data, last_sync == Fl_MIL_STD_1553::SYNC_COMMAND_STATUS ? "Command/Status" : "Data");
  } else {
    snprintf(buf, sizeof(buf), "MIL-STD-1553 parsing FAILED.\nData: 0x%X (expected 0x55AA)\nSync Type: %d (expected 0)", 
             last_data, last_sync);
  }
  
  // Test invalid parity
  uint32_t invalid_word = raw_word ^ (1U << 16); // flip parity
  int pre_count = callback_count;
  mil.feed_raw_word(invalid_word);
  
  if (callback_count == pre_count) {
    strcat(buf, "\n\nInvalid parity word correctly ignored.");
  } else {
    strcat(buf, "\n\nFAILED: Invalid parity word triggered callback.");
  }
  
  box->copy_label(buf);
  grp->end();
  
  return grp;
}

UnitTest mil1553_test(UT_TEST_MIL_STD_1553, "MIL-STD-1553", create_mil1553_test);
