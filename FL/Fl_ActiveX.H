//
// ActiveX support header file for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_ActiveX_H
#define Fl_ActiveX_H

#include <FL/Fl_Widget.H>
#include <FL/Fl_Export.H>

/**
  The Fl_ActiveX widget provides a cross-platform interface for embedding
  ActiveX controls. On non-Windows platforms, this acts as a stub so code
  compiles seamlessly cross-platform, similar to QAxWidget.
*/
class FL_EXPORT Fl_ActiveX : public Fl_Widget {
protected:
  void *control_ptr_;

  void draw() FL_OVERRIDE;
public:
  Fl_ActiveX(int X, int Y, int W, int H, const char *L = 0);
  virtual ~Fl_ActiveX();

  int handle(int event) FL_OVERRIDE;

  /**
   * Set the ActiveX control by its PROGID or CLSID (e.g. "Shell.Explorer").
   * Returns true if successful. On non-Windows platforms, this is a stub that returns true for testing.
   */
  bool set_control(const char *name);
  
  /**
   * Get the current control name.
   */
  const char *control() const { return label(); }

  /**
   * Clear the control.
   */
  void clear();

  /**
   * Get the underlying IUnknown pointer on Windows. Returns NULL on other platforms.
   */
  void *query_interface(const char *iid);
};

#endif
