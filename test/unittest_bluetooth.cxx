//
// Fl_Bluetooth test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//

#include <FL/Fl_Bluetooth.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include "unittests.h"

static Fl_Bluetooth *bt = 0;
static Fl_Text_Buffer *textbuf = 0;
static Fl_Text_Display *textdisp = 0;

static void test_cb(Fl_Widget *w, void *data) {
  if (!bt) return;
  textbuf->text("");
  
  std::string addr = Fl_Bluetooth::local_address();
  std::string name = Fl_Bluetooth::local_name();
  
  char buf[256];
  snprintf(buf, sizeof(buf), "Local Address: %s\nLocal Name: %s\n", addr.c_str(), name.c_str());
  textbuf->append(buf);
  
  textbuf->append("Scanning devices...\n");
  std::vector<Fl_Bluetooth_Device> devs = Fl_Bluetooth::scan_devices(2000);
  if (devs.empty()) {
    textbuf->append("No devices found.\n");
  } else {
    for (size_t i = 0; i < devs.size(); ++i) {
      snprintf(buf, sizeof(buf), "Found: %s (%s)\n", devs[i].address.c_str(), devs[i].name.c_str());
      textbuf->append(buf);
    }
  }
}

Fl_Widget *create_bluetooth_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();
  
  bt = new Fl_Bluetooth();
  
  Fl_Button *btn = new Fl_Button(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, 150, 30, "Test Bluetooth");
  btn->callback(test_cb);
  
  textbuf = new Fl_Text_Buffer();
  textdisp = new Fl_Text_Display(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 50, UT_TESTAREA_W - 20, UT_TESTAREA_H - 70);
  textdisp->buffer(textbuf);
  textbuf->append("Click 'Test Bluetooth' to retrieve local info and scan.\n");
  
  g->end();
  return g;
}

UnitTest bluetooth_test(UT_TEST_BLUETOOTH, "Bluetooth", create_bluetooth_test);
