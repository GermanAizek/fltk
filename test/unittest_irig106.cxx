//
// Fl_IRIG106_Ch10 test for the Fast Light Tool Kit (FLTK).
//
#include <FL/Fl_IRIG106_Ch10.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <cstring>
#include "unittests.h"

static int irig_cb_count = 0;
static uint16_t irig_last_chid = 0;
static uint8_t  irig_last_dtype = 0;
static uint32_t irig_last_plen = 0;

static void irig106_cb(Fl_IRIG106_Ch10 *bus, void *) {
  irig_cb_count++;
  Fl_IRIG106_Ch10::PacketHeader hdr = bus->last_header();
  irig_last_chid = hdr.channel_id;
  irig_last_dtype = hdr.data_type;
  irig_last_plen = hdr.packet_len;
}

static Fl_Widget* create_irig106_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  Fl_Box *box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  box->box(FL_DOWN_BOX);
  box->color(FL_WHITE);
  box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);

  Fl_IRIG106_Ch10 bus;
  bus.irig_callback(irig106_cb);

  uint8_t pkt[32];
  memset(pkt, 0, sizeof(pkt));
  pkt[0] = 0x25; pkt[1] = 0xEB; // Sync 0xEB25
  pkt[2] = 0x05; pkt[3] = 0x00; // Channel ID 5
  uint32_t plen = 32;
  memcpy(pkt + 4, &plen, 4);
  uint32_t dlen = 8;
  memcpy(pkt + 8, &dlen, 4);
  pkt[12] = 0x19; // MIL-STD-1553 format 1
  pkt[13] = 1;    // Seq 1

  bus.feed_raw_packet(pkt, 32);

  char buf[512];
  if (irig_cb_count == 1 && irig_last_chid == 5 && irig_last_dtype == 0x19 && irig_last_plen == 32) {
    snprintf(buf, sizeof(buf), "IRIG 106 Ch 10 parsing OK.\nChannel ID: %u\nData Type: 0x%02X\nPacket Len: %u B",
             irig_last_chid, irig_last_dtype, irig_last_plen);
  } else {
    snprintf(buf, sizeof(buf), "IRIG 106 Ch 10 parsing FAILED.\nCh: %u (exp 5)\nType: 0x%X (exp 0x19)",
             irig_last_chid, irig_last_dtype);
  }
  box->copy_label(buf);
  grp->end();
  return grp;
}

UnitTest irig106_test(UT_TEST_IRIG106, "IRIG 106-10", create_irig106_test);
