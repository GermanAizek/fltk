//
// Fl_Audio_Sink implementation for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Audio_Sink.H>
#include "Fl_Audio_Sink_Driver.H"
#include <stdlib.h>
#include <string.h>

Fl_Audio_Sink::Fl_Audio_Sink(const char *device_name, int sample_rate, int channels, int bit_depth)
  : driver_(0), device_(0), sample_rate_(sample_rate), channels_(channels), bit_depth_(bit_depth),
    volume_(1.0), buffer_size_(16384), bytes_written_(0), state_(StoppedState) {
  if (device_name) {
    device_ = strdup(device_name);
  }
  driver_ = Fl_Audio_Sink_Driver::new_audio_sink_driver(this);
}

Fl_Audio_Sink::~Fl_Audio_Sink() {
  state_ = StoppedState;
  if (driver_) {
    delete driver_;
    driver_ = 0;
  }
  if (device_) {
    free(device_);
    device_ = 0;
  }
}

void Fl_Audio_Sink::start() {
  state_ = ActiveState;
  if (driver_) {
    driver_->start();
  }
}

void Fl_Audio_Sink::stop() {
  state_ = StoppedState;
  if (driver_) {
    driver_->stop();
  }
}

void Fl_Audio_Sink::suspend() {
  if (state_ == ActiveState) {
    state_ = SuspendedState;
    if (driver_) {
      driver_->suspend();
    }
  }
}

void Fl_Audio_Sink::resume() {
  if (state_ == SuspendedState) {
    state_ = ActiveState;
    if (driver_) {
      driver_->resume();
    }
  }
}

void Fl_Audio_Sink::reset() {
  bytes_written_ = 0;
  state_ = StoppedState;
  if (driver_) {
    driver_->reset();
  }
}

int Fl_Audio_Sink::write(const void *data, int num_bytes) {
  if (!data || num_bytes <= 0 || state_ != ActiveState) {
    return 0;
  }
  int written = num_bytes;
  if (driver_) {
    written = driver_->write(data, num_bytes);
  }
  bytes_written_ += written;
  return written;
}

void Fl_Audio_Sink::set_volume(double vol) {
  if (vol < 0.0) vol = 0.0;
  if (vol > 1.0) vol = 1.0;
  volume_ = vol;
}

void Fl_Audio_Sink::set_buffer_size(int size) {
  if (size > 0) {
    buffer_size_ = size;
  }
}

int Fl_Audio_Sink::bytes_free() const {
  if (state_ != ActiveState) return 0;
  if (driver_) {
    return driver_->bytes_free();
  }
  return buffer_size_;
}
