//
// Fl_ARINC818 test for the Fast Light Tool Kit (FLTK).
//
#include <FL/Fl_ARINC818.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include "unittests.h"

static int a818_cb_count = 0;
static uint16_t a818_last_w = 0;
static uint16_t a818_last_h = 0;
static uint8_t  a818_last_fps = 0;

static void arinc818_cb(Fl_ARINC818 *bus, void *) {
  a818_cb_count++;
  Fl_ARINC818::ContainerInfo info = bus->last_container();
  a818_last_w = info.video_width;
  a818_last_h = info.video_height;
  a818_last_fps = info.frame_rate_fps;
}

static Fl_Widget* create_arinc818_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  Fl_Box *box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20);
  box->box(FL_DOWN_BOX);
  box->color(FL_WHITE);
  box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);

  Fl_ARINC818 bus;
  bus.arinc818_callback(arinc818_cb);

  uint8_t anc[4] = { 1, 2, 3, 4 };
  bus.feed_container(1920, 1080, 60, 100, anc, 4);

  char buf[512];
  if (a818_cb_count == 1 && a818_last_w == 1920 && a818_last_h == 1080 && a818_last_fps == 60) {
    snprintf(buf, sizeof(buf), "ARINC 818 ADVB parsing OK.\nResolution: %ux%u\nFrame Rate: %u FPS",
             a818_last_w, a818_last_h, a818_last_fps);
  } else {
    snprintf(buf, sizeof(buf), "ARINC 818 ADVB parsing FAILED.\nRes: %ux%u (exp 1920x1080)\nFPS: %u (exp 60)",
             a818_last_w, a818_last_h, a818_last_fps);
  }
  box->copy_label(buf);
  grp->end();
  return grp;
}

UnitTest arinc818_test(UT_TEST_ARINC818, "ARINC 818", create_arinc818_test);
