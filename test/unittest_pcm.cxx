//
// Fl_PCM test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_PCM.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Value_Input.H>
#include <stdio.h>
#include "unittests.h"

static Fl_PCM *pcm = 0;
static Fl_Box *status_box = 0;
static Fl_Input *dev_input = 0;
static Fl_Value_Input *rate_input = 0;
static Fl_Value_Input *channels_input = 0;
static Fl_Value_Input *bits_input = 0;

static void btn_cb(Fl_Widget* w, void*) {
  if (pcm && dev_input && status_box && rate_input && channels_input && bits_input) {
    const char *dev = dev_input->value();
    int rate = (int)rate_input->value();
    int channels = (int)channels_input->value();
    int bits = (int)bits_input->value();
    
    if (pcm->open(dev, rate, channels, bits, 1) == 0) { // open for playback
      char buf[256];
      snprintf(buf, sizeof(buf), "Opened %s successfully.\nRate: %d, Channels: %d, Bits: %d", dev, rate, channels, bits);
      status_box->copy_label(buf);
      
      // Attempt to write a tiny silent frame
      char silent_frame[8] = {0};
      pcm->write_frames(silent_frame, channels * (bits / 8));
      
      pcm->close();
    } else {
      char buf[128];
      snprintf(buf, sizeof(buf), "Failed to open %s.", dev);
      status_box->copy_label(buf);
    }
    status_box->redraw();
  }
}

static Fl_Widget* create_pcm_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  pcm = new Fl_PCM();
  
  int y_offset = UT_TESTAREA_Y + 10;
  
  dev_input = new Fl_Input(UT_TESTAREA_X + 80, y_offset, 150, 30, "Device:");
  dev_input->value("/dev/dsp");
  y_offset += 40;
  
  rate_input = new Fl_Value_Input(UT_TESTAREA_X + 80, y_offset, 150, 30, "Rate (Hz):");
  rate_input->value(44100);
  rate_input->step(1);
  y_offset += 40;
  
  channels_input = new Fl_Value_Input(UT_TESTAREA_X + 80, y_offset, 150, 30, "Channels:");
  channels_input->value(2);
  channels_input->step(1);
  y_offset += 40;
  
  bits_input = new Fl_Value_Input(UT_TESTAREA_X + 80, y_offset, 150, 30, "Bits:");
  bits_input->value(16);
  bits_input->step(8);
  y_offset += 40;
  
  status_box = new Fl_Box(UT_TESTAREA_X + 10, y_offset, UT_TESTAREA_W - 20, 80, "PCM Test");
  status_box->box(FL_DOWN_BOX);
  status_box->color(FL_WHITE);
  status_box->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
  
  y_offset += 90;
  Fl_Button *btn = new Fl_Button(UT_TESTAREA_X + 10, y_offset, 150, 40, "Test Open");
  btn->callback(btn_cb);
  
  grp->end();
  
  return grp;
}

UnitTest pcm_test(UT_TEST_PCM, "PCM Audio", create_pcm_test);
