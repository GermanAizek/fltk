//
// Fl_Gpio test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_Gpio.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include "unittests.h"

static Fl_Gpio *gpio = 0;
static Fl_Box *status_box = 0;
static int interrupt_count = 0;

static void gpio_callback(Fl_Gpio* g, void* data) {
  interrupt_count++;
  if (status_box) {
    char buf[128];
    snprintf(buf, sizeof(buf), "Interrupts: %d\nPin: %d\nVal: %d", interrupt_count, g->pin(), g->value());
    status_box->copy_label(buf);
    status_box->redraw();
  }
}

static void btn_cb(Fl_Widget* w, void*) {
  if (gpio) {
    int v = gpio->value();
    if (v >= 0) {
      gpio->value(!v); // Toggle if it's OUT
    }
  }
}

static Fl_Widget* create_gpio_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  gpio = new Fl_Gpio(21); // Example pin 21
  gpio->direction(Fl_Gpio::IN);
  gpio->edge(Fl_Gpio::BOTH);
  gpio->callback(gpio_callback);
  
  status_box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H/2 - 20, "GPIO Pin 21 Test");
  status_box->box(FL_DOWN_BOX);
  status_box->color(FL_WHITE);
  
  Fl_Button *btn = new Fl_Button(UT_TESTAREA_X + 10, UT_TESTAREA_Y + UT_TESTAREA_H/2 + 10, 150, 40, "Toggle Output");
  btn->callback(btn_cb);
  
  grp->end();
  
  return grp;
}

UnitTest gpio_test(UT_TEST_GPIO, "GPIO", create_gpio_test);
