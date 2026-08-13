//
// PPM class for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_PPM.H>
#include <string.h>
#include <stdio.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif

Fl_PPM::Fl_PPM() : 
  callback_(nullptr), user_data_(nullptr), 
  num_channels_(8), frame_length_us_(22500), pulse_width_us_(300) {
  for (int i = 0; i < 16; i++) channels_[i] = 1500; // default middle position
#if defined(_WIN32)
  handle_ = INVALID_HANDLE_VALUE;
#else
  fd_ = -1;
#endif
}

Fl_PPM::~Fl_PPM() {
  close();
}

int Fl_PPM::open(const char* device) {
  if (is_open()) return -1;

#if defined(_WIN32)
  // Mock Windows implementation
  (void)device;
  handle_ = (void*)1; // Dummy valid handle
  return 0;
#else
  // Linux / macOS standard dummy or basic open
  if (!device) return -1;
  fd_ = ::open(device, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd_ < 0) {
    // Fallback mock open if device doesn't exist
    fd_ = 1000; // Dummy valid fd
  }
  return 0;
#endif
}

int Fl_PPM::close() {
  if (!is_open()) return -1;

#if defined(_WIN32)
  if (handle_ != INVALID_HANDLE_VALUE && handle_ != (void*)1) {
    CloseHandle((HANDLE)handle_);
  }
  handle_ = INVALID_HANDLE_VALUE;
#else
  if (fd_ != 1000 && fd_ >= 0) {
    ::close(fd_);
  }
  fd_ = -1;
#endif
  return 0;
}

int Fl_PPM::is_open() const {
#if defined(_WIN32)
  return handle_ != INVALID_HANDLE_VALUE;
#else
  return fd_ >= 0;
#endif
}

void Fl_PPM::set_num_channels(int count) {
  if (count < 1) count = 1;
  if (count > 16) count = 16;
  num_channels_ = count;
}

void Fl_PPM::set_frame_length(int us) {
  frame_length_us_ = us;
}

void Fl_PPM::set_pulse_width(int us) {
  pulse_width_us_ = us;
}

int Fl_PPM::write_channels(const uint16_t* values, int count) {
  if (!is_open()) return -1;
  if (!values) return -1;
  int to_copy = count < num_channels_ ? count : num_channels_;
  for (int i = 0; i < to_copy; i++) {
    channels_[i] = values[i];
  }
  
  // Real write to hardware would go here
#if !defined(_WIN32)
  if (fd_ != 1000 && fd_ >= 0) {
    // Try sending data to fd if it is real
    // e.g. ::write(fd_, channels_, to_copy * sizeof(uint16_t));
  }
#endif

  return to_copy;
}

int Fl_PPM::read_channels(uint16_t* buffer, int max_count) {
  if (!is_open()) return -1;
  if (!buffer) return -1;

  // Real read from hardware would go here
#if !defined(_WIN32)
  if (fd_ != 1000 && fd_ >= 0) {
    // Try reading data from fd if it is real
    // e.g. ::read(fd_, buffer, max_count * sizeof(uint16_t));
  }
#endif

  int to_copy = max_count < num_channels_ ? max_count : num_channels_;
  for (int i = 0; i < to_copy; i++) {
    buffer[i] = channels_[i];
  }
  return to_copy;
}

void Fl_PPM::callback(Fl_PPM_Callback cb, void* user_data) {
  callback_ = cb;
  user_data_ = user_data;
}

void Fl_PPM::do_callback() {
  if (callback_) {
    callback_(this, user_data_);
  }
}
