//
// Fl_Audio_Output implementation for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     https://www.fltk.org/COPYING.php
//
// Please see the following page on how to report bugs and issues:
//
//     https://www.fltk.org/bugs.php
//

#include <FL/Fl_Audio_Output.H>
#include "Fl_Audio_Output_Driver.H"
#include <stdlib.h>
#include <string.h>

Fl_Audio_Output::Fl_Audio_Output(const char *device_name)
  : driver_(0), device_(0), volume_(1.0), muted_(false) {
  if (device_name) {
    device_ = strdup(device_name);
  }
  driver_ = Fl_Audio_Output_Driver::new_audio_output_driver(this);
}

Fl_Audio_Output::~Fl_Audio_Output() {
  if (driver_) {
    delete driver_;
    driver_ = 0;
  }
  if (device_) {
    free(device_);
    device_ = 0;
  }
}

void Fl_Audio_Output::set_device(const char *dev) {
  if (device_) {
    free(device_);
    device_ = 0;
  }
  if (dev) {
    device_ = strdup(dev);
  }
  if (driver_) {
    driver_->set_device(dev);
  }
}

void Fl_Audio_Output::set_volume(double vol) {
  if (vol < 0.0) vol = 0.0;
  if (vol > 1.0) vol = 1.0;
  volume_ = vol;
  if (driver_) {
    driver_->set_volume(volume_);
  }
}

void Fl_Audio_Output::set_muted(bool muted) {
  muted_ = muted;
  if (driver_) {
    driver_->set_muted(muted_);
  }
}
