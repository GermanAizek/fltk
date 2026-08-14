//
// Fl_ARINC825 test for the Fast Light Tool Kit (FLTK).
//
#include <FL/Fl_ARINC825.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"

static int a825_cb_count = 0;
static uint8_t  a825_last_lcc = 0;
static uint16_t a825_last_doc = 0;
static uint8_t  a825_last_src = 0;

static void arinc825_cb(Fl_ARINC825 *bus, void *) {
  a825_cb_count++;
  Fl_ARINC825::CanMessage msg = bus->last_message();
  a825_last_lcc = msg.lcc;
  a825_last_doc = msg.doc;
  a825_last_src = msg.src_node_id;
}

static Fl_Widget* create_arinc825_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  Fl_Box *box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  box->box(FL_DOWN_BOX);
  box->color(FL_WHITE);
  box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);

  Fl_ARINC825 bus;
  bus.arinc825_callback(arinc825_cb);

  // 29-bit CAN ID: LCC=1, SRC=12, DOC=300, RCI=0
  uint32_t can_id = (1 << 26) | (12 << 19) | (300 << 5) | (0 << 3);
  uint8_t payload[8] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  bus.feed_raw_frame(can_id, payload, 8);

  char buf[512];
  if (a825_cb_count == 1 && a825_last_lcc == 1 && a825_last_doc == 300 && a825_last_src == 12) {
    snprintf(buf, sizeof(buf), "ARINC 825 parsing OK.\nLCC: %u\nDOC: %u\nSource Node: %u",
             a825_last_lcc, a825_last_doc, a825_last_src);
  } else {
    snprintf(buf, sizeof(buf), "ARINC 825 parsing FAILED.\nLCC: %u (exp 1)\nDOC: %u (exp 300)",
             a825_last_lcc, a825_last_doc);
  }
  box->copy_label(buf);
  grp->end();
  return grp;
}

UnitTest arinc825_test(UT_TEST_ARINC825, "ARINC 825", create_arinc825_test);
