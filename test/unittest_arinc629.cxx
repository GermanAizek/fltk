//
// Fl_ARINC629 test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_ARINC629.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"

static int callback_count = 0;
static uint16_t last_data = 0;
static Fl_ARINC629::WordType last_type = Fl_ARINC629::DATA_WORD;

static void arinc_cb(Fl_ARINC629* arinc, void* d) {
  callback_count++;
  Fl_ARINC629::Word w = arinc->last_word();
  last_data = w.data;
  last_type = w.type;
}

static Fl_Widget* create_arinc629_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  Fl_Box *box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  box->box(FL_DOWN_BOX);
  box->color(FL_WHITE);
  box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
  
  Fl_ARINC629 arinc;
  arinc.arinc629_callback(arinc_cb);
  
  char buf[512];
  
  // Create a 24-bit representation of an ARINC 629 word
  // Sync = 5 (Command), Data = 0xABCD
  // 16-bit data + 1 parity bit = 17 bits for parity check
  // Parity needs to be odd.
  uint32_t raw_word = 0xABCD | (5U << 17);
  
  // Count ones in lower 16 bits
  int ones = 0;
  for(int i=0; i<16; i++) {
    if(raw_word & (1U << i)) ones++;
  }
  if((ones % 2) == 0) {
    // Set parity bit (bit 16)
    raw_word |= (1U << 16);
  }
  
  arinc.feed_raw_word(raw_word);
  
  if (callback_count == 1 && last_data == 0xABCD && last_type == Fl_ARINC629::COMMAND_WORD) {
    snprintf(buf, sizeof(buf), "ARINC 629 parsing OK.\nData: 0x%X\nType: %s", 
             last_data, last_type == Fl_ARINC629::COMMAND_WORD ? "Command" : "Data");
  } else {
    snprintf(buf, sizeof(buf), "ARINC 629 parsing FAILED.\nData: 0x%X (expected 0xABCD)\nType: %d (expected 1)", 
             last_data, last_type);
  }
  
  // Test invalid parity
  uint32_t invalid_word = raw_word ^ (1U << 16); // flip parity
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

UnitTest arinc629_test(UT_TEST_ARINC629, "ARINC 629", create_arinc629_test);
