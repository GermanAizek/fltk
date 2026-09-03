//
// ActiveX support implementation for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_ActiveX.H>
#include <FL/fl_draw.H>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <ole2.h>
#endif

Fl_ActiveX::Fl_ActiveX(int X, int Y, int W, int H, const char *L)
  : Fl_Widget(X, Y, W, H, L), control_ptr_(0) {
}

Fl_ActiveX::~Fl_ActiveX() {
  clear();
}

void Fl_ActiveX::clear() {
  copy_label(0);
#ifdef _WIN32
  if (control_ptr_) {
    ((IUnknown*)control_ptr_)->Release();
  }
#endif
  control_ptr_ = 0;
}

bool Fl_ActiveX::set_control(const char *name) {
  clear();
  if (name) {
    copy_label(name);
#ifdef _WIN32
    int len = MultiByteToWideChar(CP_UTF8, 0, name, -1, NULL, 0);
    if (len > 0) {
      wchar_t *wname = new wchar_t[len];
      MultiByteToWideChar(CP_UTF8, 0, name, -1, wname, len);
      
      CLSID clsid;
      if (CLSIDFromProgID(wname, &clsid) == S_OK || CLSIDFromString(wname, &clsid) == S_OK) {
        IUnknown *pUnk = NULL;
        CoInitialize(NULL);
        if (CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IUnknown, (void**)&pUnk) == S_OK) {
          control_ptr_ = pUnk;
          delete[] wname;
          return true;
        }
      }
      delete[] wname;
    }
    return false;
#else
    return true; // Stub for testing on non-Windows
#endif
  }
  return false;
}

void *Fl_ActiveX::query_interface(const char *iid) {
#ifdef _WIN32
  if (!control_ptr_ || !iid) return 0;
  
  int len = MultiByteToWideChar(CP_UTF8, 0, iid, -1, NULL, 0);
  if (len > 0) {
    wchar_t *wiid = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, iid, -1, wiid, len);
    
    IID riid;
    if (IIDFromString(wiid, &riid) == S_OK) {
      void *pResult = 0;
      ((IUnknown*)control_ptr_)->QueryInterface(riid, &pResult);
      delete[] wiid;
      return pResult;
    }
    delete[] wiid;
  }
#endif
  return 0;
}

void Fl_ActiveX::draw() {
  draw_box();
  if (label()) {
    fl_color(FL_BLACK);
    fl_font(FL_HELVETICA, 12);
    fl_draw(label(), x(), y(), w(), h(), FL_ALIGN_CENTER);
  }
}

int Fl_ActiveX::handle(int event) {
  return Fl_Widget::handle(event);
}
