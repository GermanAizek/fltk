//
// FLTK NFC (Near Field Communication) implementation
//
// Copyright 2026 by GermanAizek.
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

#include <FL/Fl_NFC.H>
#include "Fl_NFC_Driver.H"

#if defined(_WIN32)
#include "drivers/WinAPI/Fl_WinAPI_NFC_Driver.H"
#elif defined(__APPLE__)
#include "drivers/Cocoa/Fl_Cocoa_NFC_Driver.H"
#else
#include "drivers/X11/Fl_X11_NFC_Driver.H"
#endif

// --- Fl_NFC_Record ---
Fl_NFC_Record::Fl_NFC_Record(const std::string& type, const std::string& payload)
  : type_(type), payload_(payload) {
}

// --- Fl_NFC_Manager ---
Fl_NFC_Manager::Fl_NFC_Manager()
  : target_detected_cb_(0), target_lost_cb_(0), user_data_(0) {
  driver_ = Fl_NFC_Driver::create(this);
}

Fl_NFC_Manager::~Fl_NFC_Manager() {
  delete driver_;
}

int Fl_NFC_Manager::start_target_detection() {
  if (driver_) return driver_->start_target_detection();
  return 0;
}

void Fl_NFC_Manager::stop_target_detection() {
  if (driver_) driver_->stop_target_detection();
}

void Fl_NFC_Manager::target_detected_callback(Fl_NFC_Target_Callback cb, void* data) {
  target_detected_cb_ = cb;
  user_data_ = data;
}

void Fl_NFC_Manager::target_lost_callback(Fl_NFC_Target_Callback cb, void* data) {
  target_lost_cb_ = cb;
  // Use same user_data_ or introduce separate. Here we share for simplicity.
  if (data) user_data_ = data;
}

void Fl_NFC_Manager::handle_target_detected(Fl_NFC_Target* target) {
  if (target_detected_cb_) {
    target_detected_cb_(target, user_data_);
  }
}

void Fl_NFC_Manager::handle_target_lost(Fl_NFC_Target* target) {
  if (target_lost_cb_) {
    target_lost_cb_(target, user_data_);
  }
}

// --- Fl_NFC_Driver Factory ---
Fl_NFC_Driver* Fl_NFC_Driver::create(Fl_NFC_Manager* m) {
#if defined(_WIN32)
  return new Fl_WinAPI_NFC_Driver(m);
#elif defined(__APPLE__)
  return new Fl_Cocoa_NFC_Driver(m);
#else
  return new Fl_X11_NFC_Driver(m);
#endif
}
