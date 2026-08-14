//
// Fl_ARINC717 test for the Fast Light Tool Kit (FLTK).
//
#include <FL/Fl_ARINC717.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"

static int a717_cb_count = 0;
static uint16_t a717_last_sync = 0;
static uint8_t  a717_last_sf = 0;
static uint16_t a717_last_word1 = 0;

static void arinc717_cb(Fl_ARINC717 *bus, void *) {
  a717_cb_count++;
  Fl_ARINC717::Subframe sf = bus->last_subframe();
  a717_last_sync = sf.sync_word;
  a717_last_sf = sf.subframe_num;
  if (sf.word_count > 1) a717_last_word1 = sf.words[1];
}

static Fl_Widget* create_arinc717_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  Fl_Box *box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  box->box(FL_DOWN_BOX);
  box->color(FL_WHITE);
  box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);

  Fl_ARINC717 bus;
  bus.arinc717_callback(arinc717_cb);

  uint16_t words[64];
  words[0] = 0x247; // Subframe 1 sync
  words[1] = 0x0ABC; // Altitude sample
  bus.feed_subframe(1, words, 64);

  char buf[512];
  if (a717_cb_count == 1 && a717_last_sync == 0x247 && a717_last_sf == 1 && a717_last_word1 == 0x0ABC) {
    snprintf(buf, sizeof(buf), "ARINC 717 parsing OK.\nSync Word: 0x%03X\nSubframe: %u\nWord 1: 0x%03X",
             a717_last_sync, a717_last_sf, a717_last_word1);
  } else {
    snprintf(buf, sizeof(buf), "ARINC 717 parsing FAILED.\nSync: 0x%X (exp 0x247)\nSF: %u (exp 1)",
             a717_last_sync, a717_last_sf);
  }
  box->copy_label(buf);
  grp->end();
  return grp;
}

UnitTest arinc717_test(UT_TEST_ARINC717, "ARINC 717", create_arinc717_test);
