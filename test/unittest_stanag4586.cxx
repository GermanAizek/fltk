//
// Fl_STANAG4586 test for the Fast Light Tool Kit (FLTK).
//
#include <FL/Fl_STANAG4586.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"

static int stanag_cb_count = 0;
static uint16_t stanag_last_msgid = 0;
static uint32_t stanag_last_veh = 0;
static uint16_t stanag_last_sub = 0;

static void stanag4586_cb(Fl_STANAG4586 *bus, void *) {
  stanag_cb_count++;
  Fl_STANAG4586::DliMessage msg = bus->last_message();
  stanag_last_msgid = msg.message_id;
  stanag_last_veh = msg.vehicle_id;
  stanag_last_sub = msg.subsystem_id;
}

static Fl_Widget* create_stanag4586_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  Fl_Box *box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  box->box(FL_DOWN_BOX);
  box->color(FL_WHITE);
  box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);

  Fl_STANAG4586 bus;
  bus.stanag4586_callback(stanag4586_cb);

  uint8_t payload[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };
  bus.feed_message(100, 501, 12, payload, 8); // DLI Msg #100

  char buf[512];
  if (stanag_cb_count == 1 && stanag_last_msgid == 100 && stanag_last_veh == 501 && stanag_last_sub == 12) {
    snprintf(buf, sizeof(buf), "NATO STANAG 4586 parsing OK.\nMessage ID: #%u\nVehicle ID: %u\nSubsystem ID: %u",
             stanag_last_msgid, stanag_last_veh, stanag_last_sub);
  } else {
    snprintf(buf, sizeof(buf), "STANAG 4586 parsing FAILED.\nMsgID: #%u (exp 100)\nVeh: %u (exp 501)",
             stanag_last_msgid, stanag_last_veh);
  }
  box->copy_label(buf);
  grp->end();
  return grp;
}

UnitTest stanag4586_test(UT_TEST_STANAG4586, "STANAG 4586", create_stanag4586_test);
