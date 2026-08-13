//
// FLTK NFC Linux/X11 Driver Implementation
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
#include "Fl_X11_NFC_Driver.H"
#include <FL/Fl.H>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_PCSC_WINSCARD_H
#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>
#endif

Fl_X11_NFC_Driver::Fl_X11_NFC_Driver(Fl_NFC_Manager* m)
  : Fl_NFC_Driver(m), context_(0) {
}

Fl_X11_NFC_Driver::~Fl_X11_NFC_Driver() {
  stop_target_detection();
}

void Fl_X11_NFC_Driver::timer_cb(void* data) {
  Fl_X11_NFC_Driver* self = (Fl_X11_NFC_Driver*)data;
#ifdef HAVE_PCSC_WINSCARD_H
  if (!self->context_) return;
  
  SCARD_READERSTATE rs;
  memset(&rs, 0, sizeof(rs));
  rs.szReader = "\\\\?PnP?\\Notification";
  rs.dwCurrentState = SCARD_STATE_UNAWARE;
  
  // Quick poll
  LONG rv = SCardGetStatusChange((SCARDCONTEXT)self->context_, 0, &rs, 1);
  if (rv == SCARD_S_SUCCESS) {
    if (rs.dwEventState & SCARD_STATE_PRESENT) {
      // Just emit a fake target for the stub
      Fl_NFC_Target target;
      self->manager_->handle_target_detected(&target);
    } else if (rs.dwEventState & SCARD_STATE_EMPTY) {
      Fl_NFC_Target target;
      self->manager_->handle_target_lost(&target);
    }
  }
#endif
  Fl::repeat_timeout(0.5, timer_cb, self);
}

int Fl_X11_NFC_Driver::start_target_detection() {
#ifdef HAVE_PCSC_WINSCARD_H
  LONG rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, (SCARDCONTEXT*)&context_);
  if (rv != SCARD_S_SUCCESS) {
    return 0; // Failed
  }
  Fl::add_timeout(0.5, timer_cb, this);
  return 1;
#else
  return 0; // Not supported
#endif
}

void Fl_X11_NFC_Driver::stop_target_detection() {
#ifdef HAVE_PCSC_WINSCARD_H
  if (context_) {
    Fl::remove_timeout(timer_cb, this);
    SCardReleaseContext((SCARDCONTEXT)context_);
    context_ = 0;
  }
#endif
}
