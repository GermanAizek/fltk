//
// Fl_ADSB_1090ES test for the Fast Light Tool Kit (FLTK).
//
#include <FL/Fl_ADSB_1090ES.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <cstring>
#include "unittests.h"

static int adsb_cb_count = 0;
static uint32_t adsb_last_icao = 0;
static char     adsb_last_cs[16] = {0};
static double   adsb_last_alt = 0;

static void adsb1090_cb(Fl_ADSB_1090ES *bus, void *) {
  adsb_cb_count++;
  Fl_ADSB_1090ES::SquitterMsg msg = bus->last_message();
  adsb_last_icao = msg.icao24;
  snprintf(adsb_last_cs, sizeof(adsb_last_cs), "%s", msg.callsign);
  adsb_last_alt = msg.altitude_ft;
}

static Fl_Widget* create_adsb1090_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  Fl_Box *box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  box->box(FL_DOWN_BOX);
  box->color(FL_WHITE);
  box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);

  Fl_ADSB_1090ES bus;
  bus.adsb_callback(adsb1090_cb);

  bus.feed_squitter(0xA12345, "TEST01", 32000.0, 450.0, 270.0, 0.0, 7700);

  char buf[512];
  if (adsb_cb_count == 1 && adsb_last_icao == 0xA12345 && strcmp(adsb_last_cs, "TEST01") == 0 && adsb_last_alt == 32000.0) {
    snprintf(buf, sizeof(buf), "ADS-B 1090ES parsing OK.\nICAO: 0x%06X\nCallsign: %s\nAltitude: %.0f FT",
             adsb_last_icao, adsb_last_cs, adsb_last_alt);
  } else {
    snprintf(buf, sizeof(buf), "ADS-B 1090ES parsing FAILED.\nICAO: 0x%X (exp 0xA12345)\nCallsign: %s",
             adsb_last_icao, adsb_last_cs);
  }
  box->copy_label(buf);
  grp->end();
  return grp;
}

UnitTest adsb1090_test(UT_TEST_ADSB1090, "ADS-B 1090", create_adsb1090_test);
