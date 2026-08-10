//
// Fl_XBM_Image routines.
//
// Copyright 1997-2026 by Bill Spitzak and others.
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
// Contents:
//
//   Fl_XBM_Image::Fl_XBM_Image() - Load an XBM file.
//

//
// Include necessary header files...
//

#include <FL/Fl_XBM_Image.H>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <FL/fl_utf8.h>
#include "flstring.h"

//
// 'Fl_XBM_Image::Fl_XBM_Image()' - Load an XBM file.
//

/**
  The constructor loads the named XBM file from the given name filename.

  The destructor frees all memory and server resources that are used by
  the image.
*/
Fl_XBM_Image::Fl_XBM_Image(const char *name) : Fl_Bitmap((const char *)0,0,0) {
  std::ifstream f;
  uchar *ptr;

  f.open(name, std::ios::binary);
  if (!f.is_open()) return;

  char buffer[1024];
  char junk[1024];
  int wh[2]; // width and height
  int i;
  for (i = 0; i<2; i++) {
    for (;;) {
      if (!f.getline(buffer, 1024)) {
        f.close();
        return;
      }
      int r = 0;
      std::string dummy1, dummy2;
      if ((std::istringstream(buffer) >> dummy1 >> dummy2 >> wh[i]) && dummy1 == "#define") r = 2;
      if (r >= 2) break;
    }
  }

  // skip to data array:
  for (;;) {
    if (!f.getline(buffer, 1024)) {
      f.close();
      return;
    }
    if (!strncmp(buffer,"static ",7)) break;
  }

  // Allocate memory...
  w(wh[0]);
  h(wh[1]);

  int n = ((wh[0]+7)/8)*wh[1];
  array = new uchar[n];

  // read the data:
  for (i = 0, ptr = (uchar *)array; i < n;) {
    if (!f.getline(buffer, 1024)) {
      f.close();
      return;
    }
    const char *a = buffer;
    while (*a && i<n) {
      unsigned int t;
      std::string chunk(a);
      size_t pos = chunk.find("0x");
      if (pos != std::string::npos && (std::istringstream(chunk.substr(pos)) >> std::hex >> t)) {
        *ptr++ = (uchar)t;
        i ++;
      }
      while (*a && *a++ != ',') {/*empty*/}
    }
  }

  f.close();
}
