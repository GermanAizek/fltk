//
// File loading routines for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2010 by Bill Spitzak and others.
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

#include <FL/Fl.H>
#include <FL/Fl_Browser.H>
#include <FL/fl_utf8.h>
#include <fstream>
#include <string>

/**
  Clears the browser and reads the file, adding each line from the file
  to the browser.  If the filename is NULL or a zero-length
  string then this just clears the browser.  This returns zero if there
  was any error in opening or reading the file, in which case errno
  is set to the system error.  The data() of each line is set
  to NULL.
  \param[in] filename The filename to load
  \returns 1 if OK, 0 on error (errno has reason)
  \see add()
*/
int Fl_Browser::load(const char *filename) {
#define MAXFL_BLINE 1024
  char newtext[MAXFL_BLINE];
  int i = 0;
  clear();
  if (!filename || !(filename[0])) return 1;

#if defined(_WIN32)
  // fl_fopen or conversion for Windows UTF-8 wide path support
  unsigned short wbuf[1024];
  fl_utf8towc(filename, (unsigned int)strlen(filename), wbuf, 1024);
  std::ifstream file(reinterpret_cast<const wchar_t*>(wbuf), std::ios::in | std::ios::binary);
#else
  std::ifstream file(filename, std::ios::in | std::ios::binary);
#endif

  if (!file.is_open()) return 0;

  char ch;
  while (file.get(ch)) {
    if (ch == '\n' || ch == '\0' || i >= (MAXFL_BLINE - 1)) {
      newtext[i] = '\0';
      add(newtext);
      i = 0;
    } else {
      newtext[i++] = ch;
    }
  }

  if (i > 0) {
    newtext[i] = '\0';
    add(newtext);
  }

  return 1;
}