//
// Fl_Cyphal test for the Fast Light Tool Kit (FLTK).
//
#include <FL/Fl_Cyphal.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"

static int cy_cb_count = 0;
static uint8_t  cy_last_prio = 0;
static uint16_t cy_last_port = 0;
static uint8_t  cy_last_src = 0;

static void cyphal_cb(Fl_Cyphal *bus, void *) {
  cy_cb_count++;
  Fl_Cyphal::Transfer tf = bus->last_transfer();
  cy_last_prio = tf.priority;
  cy_last_port = tf.port_id;
  cy_last_src = tf.src_node_id;
}

static Fl_Widget* create_cyphal_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  Fl_Box *box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  box->box(FL_DOWN_BOX);
  box->color(FL_WHITE);
  box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);

  Fl_Cyphal bus;
  bus.cyphal_callback(cyphal_cb);

  uint8_t payload[4] = { 0x11, 0x22, 0x33, 0x44 };
  bus.feed_transfer(3, 750, false, 42, 255, 0, payload, 4);

  char buf[512];
  if (cy_cb_count == 1 && cy_last_prio == 3 && cy_last_port == 750 && cy_last_src == 42) {
    snprintf(buf, sizeof(buf), "OpenCyphal UAVCAN parsing OK.\nPriority: %u\nSubject ID: %u\nSource Node: %u",
             cy_last_prio, cy_last_port, cy_last_src);
  } else {
    snprintf(buf, sizeof(buf), "Cyphal parsing FAILED.\nPrio: %u (exp 3)\nPort: %u (exp 750)",
             cy_last_prio, cy_last_port);
  }
  box->copy_label(buf);
  grp->end();
  return grp;
}

UnitTest cyphal_test(UT_TEST_CYPHAL, "Cyphal / UAVCAN", create_cyphal_test);
