//
// Fl_PWM test for the Fast Light Tool Kit (FLTK).
//
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_PWM.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Light_Button.H>
#include "unittests.h"

static Fl_PWM *pwm = 0;
static Fl_Box *status_box = 0;

static void update_status() {
  if (status_box && pwm) {
    char buf[128];
    snprintf(buf, sizeof(buf), "Chip: %d\nChannel: %d\nPeriod: %ld\nDuty: %ld\nEnabled: %d", 
             pwm->chip(), pwm->channel(), pwm->period(), pwm->duty_cycle(), pwm->enable());
    status_box->copy_label(buf);
    status_box->redraw();
  }
}

static void slider_cb(Fl_Widget* w, void*) {
  if (pwm) {
    Fl_Value_Slider *s = (Fl_Value_Slider*)w;
    pwm->duty_cycle_percent(s->value() / 100.0);
    update_status();
  }
}

static void enable_cb(Fl_Widget* w, void*) {
  if (pwm) {
    Fl_Light_Button *b = (Fl_Light_Button*)w;
    pwm->enable(b->value());
    update_status();
  }
}

static Fl_Widget* create_pwm_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  pwm = new Fl_PWM(0, 0); // Example chip 0, channel 0
  pwm->period(10000000); // 10ms period
  pwm->duty_cycle(0);
  
  status_box = new Fl_Box(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H/3, "PWM Test");
  status_box->box(FL_DOWN_BOX);
  status_box->color(FL_WHITE);
  
  Fl_Value_Slider *slider = new Fl_Value_Slider(UT_TESTAREA_X + 10, UT_TESTAREA_Y + UT_TESTAREA_H/3 + 20, UT_TESTAREA_W - 20, 30, "Duty Cycle (%)");
  slider->type(FL_HOR_SLIDER);
  slider->bounds(0, 100);
  slider->value(0);
  slider->callback(slider_cb);
  slider->align(FL_ALIGN_TOP);
  
  Fl_Light_Button *enable_btn = new Fl_Light_Button(UT_TESTAREA_X + 10, UT_TESTAREA_Y + UT_TESTAREA_H/3 + 80, 150, 40, "Enable PWM");
  enable_btn->callback(enable_cb);
  enable_btn->value(pwm->enable() > 0 ? 1 : 0);
  
  grp->end();
  
  update_status();
  
  return grp;
}

UnitTest pwm_test(UT_TEST_PWM, "PWM / Analog", create_pwm_test);
