//
// Fl_ARINC708 test for the Fast Light Tool Kit (FLTK).
//
#include <FL/Fl_ARINC708.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"

static int a708_cb_count = 0;
static float a708_last_scan = 0;
static uint16_t a708_last_rng = 0;

static void arinc708_cb(Fl_ARINC708 *bus, void *) {
  a708_cb_count++;
  Fl_ARINC708::RadarRadial rad = bus->last_radial();
  a708_last_scan = rad.scan_angle_deg;
  a708_last_rng = rad.range_nm;
}

static Fl_Widget* create_arinc708_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  Fl_Box *box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  box->box(FL_DOWN_BOX);
  box->color(FL_WHITE);
  box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);

  Fl_ARINC708 bus;
  bus.arinc708_callback(arinc708_cb);

  uint8_t bins[64] = { 1, 2, 3, 0 };
  bus.feed_radial(25.5f, 0.0f, 40, 1, bins, 64);

  char buf[512];
  if (a708_cb_count == 1 && a708_last_scan == 25.5f && a708_last_rng == 40) {
    snprintf(buf, sizeof(buf), "ARINC 708 WXR parsing OK.\nScan Angle: %+.1f°\nRange Scale: %u NM",
             a708_last_scan, a708_last_rng);
  } else {
    snprintf(buf, sizeof(buf), "ARINC 708 WXR parsing FAILED.\nScan: %+.1f (exp 25.5)\nRange: %u (exp 40)",
             a708_last_scan, a708_last_rng);
  }
  box->copy_label(buf);
  grp->end();
  return grp;
}

UnitTest arinc708_test(UT_TEST_ARINC708, "ARINC 708", create_arinc708_test);
