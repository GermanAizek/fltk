//
// Fl_Camera test for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include <FL/Fl_Camera.H>
#include <FL/Fl_Group.H>
#include "unittests.h"

static Fl_Camera *camera = 0;

static Fl_Widget* create_camera_test() {
  Fl_Group *grp = new Fl_Group(UT_TESTAREA_X, UT_TESTAREA_Y, UT_TESTAREA_W, UT_TESTAREA_H);
  
  camera = new Fl_Camera(UT_TESTAREA_X + 10, UT_TESTAREA_Y + 10, UT_TESTAREA_W - 20, UT_TESTAREA_H - 20, "USB Camera");
  
  grp->end();
  
  // start playing automatically in the test
  camera->start();
  
  return grp;
}

UnitTest camera_test(UT_TEST_CAMERA, "Camera", create_camera_test);
