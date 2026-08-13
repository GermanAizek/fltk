//
// Fl_DBus test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//

#include <FL/Fl_DBus.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include "unittests.h"

static Fl_DBus *dbus_sys = 0;
static Fl_DBus *dbus_sess = 0;
static Fl_Text_Buffer *textbuf = 0;
static Fl_Text_Display *textdisp = 0;

static void test_cb(Fl_Widget *w, void *data) {
  if (!dbus_sys || !dbus_sess) return;
  textbuf->text("");

  char buf[256];
  snprintf(buf, sizeof(buf), "System Bus connected: %s\n", dbus_sys->is_connected() ? "yes" : "no");
  textbuf->append(buf);

  snprintf(buf, sizeof(buf), "Session Bus connected: %s\n", dbus_sess->is_connected() ? "yes" : "no");
  textbuf->append(buf);

  textbuf->append("\nCalling GetMachineId on System Bus (org.freedesktop.DBus)...\n");
  std::string machine_id = dbus_sys->call_method("org.freedesktop.DBus", 
                                                 "/org/freedesktop/DBus", 
                                                 "org.freedesktop.DBus.Peer", 
                                                 "GetMachineId");
  
  if (!machine_id.empty()) {
    snprintf(buf, sizeof(buf), "Machine ID: %s\n", machine_id.c_str());
    textbuf->append(buf);
  } else {
    textbuf->append("Failed to get Machine ID or method not supported.\n");
  }
}

Fl_Widget *create_dbus_test() {
  Fl_Group *g = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  g->begin();
  
  dbus_sys = new Fl_DBus(Fl_DBus::SystemBus);
  dbus_sess = new Fl_DBus(Fl_DBus::SessionBus);
  
  Fl_Button *btn = new Fl_Button(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, 150, 30, "Test DBus");
  btn->callback(test_cb);
  
  textbuf = new Fl_Text_Buffer();
  textdisp = new Fl_Text_Display(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 50, UT_TESTAREA_W - 20, UT_TESTAREA_H - 70);
  textdisp->buffer(textbuf);
  textbuf->append("Click 'Test DBus' to test connection and basic method call.\n");
  
  g->end();
  return g;
}

UnitTest dbus_test(UT_TEST_DBUS, "DBus", create_dbus_test);
