//
// Fl_MAVLink2 test for the Fast Light Tool Kit (FLTK).
//
#include <FL/Fl_MAVLink2.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"

static int mav_cb_count = 0;
static uint8_t  mav_last_sys = 0;
static uint8_t  mav_last_comp = 0;
static uint32_t mav_last_msgid = 0;

static void mavlink2_cb(Fl_MAVLink2 *bus, void *) {
  mav_cb_count++;
  Fl_MAVLink2::Message msg = bus->last_message();
  mav_last_sys = msg.sys_id;
  mav_last_comp = msg.comp_id;
  mav_last_msgid = msg.msg_id;
}

static Fl_Widget* create_mavlink2_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  Fl_Box *box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  box->box(FL_DOWN_BOX);
  box->color(FL_WHITE);
  box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);

  Fl_MAVLink2 bus;
  bus.mavlink_callback(mavlink2_cb);

  uint8_t payload[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
  bus.feed_message(1, 200, 30, payload, 8); // Msg #30 ATTITUDE

  char buf[512];
  if (mav_cb_count == 1 && mav_last_sys == 1 && mav_last_comp == 200 && mav_last_msgid == 30) {
    snprintf(buf, sizeof(buf), "MAVLink 2.0 parsing OK.\nSystem ID: %u\nComponent ID: %u\nMessage ID: #%u",
             mav_last_sys, mav_last_comp, mav_last_msgid);
  } else {
    snprintf(buf, sizeof(buf), "MAVLink 2.0 parsing FAILED.\nSys: %u (exp 1)\nMsgID: #%u (exp 30)",
             mav_last_sys, mav_last_msgid);
  }
  box->copy_label(buf);
  grp->end();
  return grp;
}

UnitTest mavlink2_test(UT_TEST_MAVLINK2, "MAVLink 2", create_mavlink2_test);
