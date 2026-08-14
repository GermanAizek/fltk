//
// Fl_SpaceWire test for the Fast Light Tool Kit (FLTK).
//
#include <FL/Fl_SpaceWire.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"

static int spw_cb_count = 0;
static uint8_t  spw_last_addr = 0;
static uint8_t  spw_last_pid = 0;
static uint32_t spw_last_len = 0;

static void spacewire_cb(Fl_SpaceWire *bus, void *) {
  spw_cb_count++;
  Fl_SpaceWire::Packet pkt = bus->last_packet();
  spw_last_addr = pkt.logical_address;
  spw_last_pid = pkt.protocol_id;
  spw_last_len = pkt.length;
}

static Fl_Widget* create_spacewire_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  Fl_Box *box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  box->box(FL_DOWN_BOX);
  box->color(FL_WHITE);
  box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);

  Fl_SpaceWire bus;
  bus.spacewire_callback(spacewire_cb);

  uint8_t data[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
  bus.feed_raw_packet(0xFE, 0x01, data, 4);

  char buf[512];
  if (spw_cb_count == 1 && spw_last_addr == 0xFE && spw_last_pid == 0x01 && spw_last_len == 4) {
    snprintf(buf, sizeof(buf), "SpaceWire ECSS parsing OK.\nLogical Addr: 0x%02X\nProtocol ID: 0x%02X\nData Length: %u B",
             spw_last_addr, spw_last_pid, spw_last_len);
  } else {
    snprintf(buf, sizeof(buf), "SpaceWire ECSS parsing FAILED.\nAddr: 0x%X (exp 0xFE)\nPID: 0x%X (exp 0x01)",
             spw_last_addr, spw_last_pid);
  }
  box->copy_label(buf);
  grp->end();
  return grp;
}

UnitTest spacewire_test(UT_TEST_SPACEWIRE, "SpaceWire", create_spacewire_test);
