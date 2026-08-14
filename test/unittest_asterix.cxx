//
// Fl_ASTERIX test for the Fast Light Tool Kit (FLTK).
//
#include <FL/Fl_ASTERIX.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"

static int astx_cb_count = 0;
static uint8_t  astx_last_cat = 0;
static uint32_t astx_last_icao = 0;
static double   astx_last_gs = 0;

static void asterix_cb(Fl_ASTERIX *bus, void *) {
  astx_cb_count++;
  Fl_ASTERIX::Cat021Record rec = bus->last_record();
  astx_last_cat = rec.category;
  astx_last_icao = rec.target_address;
  astx_last_gs = rec.ground_speed_kt;
}

static Fl_Widget* create_asterix_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  Fl_Box *box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  box->box(FL_DOWN_BOX);
  box->color(FL_WHITE);
  box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);

  Fl_ASTERIX bus;
  bus.asterix_callback(asterix_cb);

  bus.feed_cat021(0x19, 0x01, 0x484042, 55.75, 37.61, 350.0, 480.0, 90.0, 7700, "AFL123");

  char buf[512];
  if (astx_cb_count == 1 && astx_last_cat == 21 && astx_last_icao == 0x484042 && astx_last_gs == 480.0) {
    snprintf(buf, sizeof(buf), "ASTERIX Cat 021 parsing OK.\nCategory: %u\nICAO: 0x%06X\nGround Speed: %.0f KT",
             astx_last_cat, astx_last_icao, astx_last_gs);
  } else {
    snprintf(buf, sizeof(buf), "ASTERIX parsing FAILED.\nCat: %u (exp 21)\nICAO: 0x%X (exp 0x484042)",
             astx_last_cat, astx_last_icao);
  }
  box->copy_label(buf);
  grp->end();
  return grp;
}

UnitTest asterix_test(UT_TEST_ASTERIX, "ASTERIX", create_asterix_test);
