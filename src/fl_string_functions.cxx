/*
* Platform agnostic string portability functions for the Fast Light Tool Kit (FLTK).
 *
 * Copyright 2020-2026 by Bill Spitzak and others.
 *
 * This library is free software. Distribution and use rights are outlined in
 * the file "COPYING" which should have been included with this file.  If this
 * file is missing or damaged, see the license at:
 *
 *     https://www.fltk.org/COPYING.php
 *
 * Please see the following page on how to report bugs and issues:
 *
 *     https://www.fltk.org/bugs.php
 */

#include <FL/fl_string_functions.h>
#include "Fl_System_Driver.H"

#include <algorithm>
#include <cstddef>

/**
  Cross platform interface to POSIX function strdup().

  The fl_strdup() function returns a pointer to a new string which is
  a duplicate of the string 's'. Memory for the new string is obtained
  with malloc(3), and can be freed with free(3).

  Implementation:
    - POSIX: strdup()
    - WinAPI: _strdup()
 */
char *fl_strdup(const char *s) {
  if (s == nullptr) {
    return nullptr;
  }
  return Fl::system_driver()->strdup(s);
}

/*
 * 'fl_strlcpy()' - Safely copy two strings.
 */
size_t                          /* O - Length of source string */
fl_strlcpy(char       *dst,     /* O - Destination string */
           const char *src,     /* I - Source string */
           const size_t size) { /* I - Size of destination string buffer */
  if (src == nullptr) {
    if (dst != nullptr && size != 0U) {
      dst[0] = '\0';
    }
    return 0U;
  }

  // Calculate string length without <cstring> strlen
  const char *src_end = src;
  while (*src_end != '\0') {
    ++src_end;
  }
  const auto srclen = static_cast<size_t>(src_end - src);

  if (dst != nullptr && size != 0U) {
    const size_t copylen = srclen >= size ? size - 1U : srclen;
    (void)std::copy_n(src, copylen, dst);
    dst[copylen] = '\0';
  }

  return srclen;
}