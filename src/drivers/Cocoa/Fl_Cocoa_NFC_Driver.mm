//
// FLTK NFC macOS/Cocoa Driver Implementation
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

#include <config.h>
#include "Fl_Cocoa_NFC_Driver.H"
#include <FL/Fl.H>
#include <stdio.h>
#include <string.h>

#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>

Fl_Cocoa_NFC_Driver::Fl_Cocoa_NFC_Driver(Fl_NFC_Manager* m)
  : Fl_NFC_Driver(m), context_(0) {
}

Fl_Cocoa_NFC_Driver::~Fl_Cocoa_NFC_Driver() {
  stop_target_detection();
}

void Fl_Cocoa_NFC_Driver::timer_cb(void* data) {
  Fl_Cocoa_NFC_Driver* self = (Fl_Cocoa_NFC_Driver*)data;
  if (!self->context_) return;
  
  SCARD_READERSTATE rs;
  memset(&rs, 0, sizeof(rs));
  rs.szReader = "\\\\?PnP?\\Notification";
  rs.dwCurrentState = SCARD_STATE_UNAWARE;
  
  // Quick poll
  LONG rv = SCardGetStatusChange((SCARDCONTEXT)self->context_, 0, &rs, 1);
  if (rv == SCARD_S_SUCCESS) {
    if (rs.dwEventState & SCARD_STATE_PRESENT) {
      Fl_NFC_Target target;
      target.type = 1;
      self->manager_->handle_target_detected(&target);
    } else if (rs.dwEventState & SCARD_STATE_EMPTY) {
      Fl_NFC_Target target;
      target.type = 1;
      self->manager_->handle_target_lost(&target);
    }
  }
  
  Fl::repeat_timeout(0.5, timer_cb, self);
}

int Fl_Cocoa_NFC_Driver::start_target_detection() {
  LONG rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, (SCARDCONTEXT*)&context_);
  if (rv != SCARD_S_SUCCESS) {
    return 0; // Failed
  }
  Fl::add_timeout(0.5, timer_cb, this);
  return 1;
}

void Fl_Cocoa_NFC_Driver::stop_target_detection() {
  if (context_) {
    Fl::remove_timeout(timer_cb, this);
    SCardReleaseContext((SCARDCONTEXT)context_);
    context_ = 0;
  }
}
