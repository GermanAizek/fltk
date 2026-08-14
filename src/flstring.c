/*
 * BSD string functions for the Fast Light Tool Kit (FLTK).
 *
 * Copyright 1998-2010 by Bill Spitzak and others.
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

#include "flstring.h"


/*
 * 'fl_strlcat()' - Safely concatenate two strings.
 */

size_t                          /* O - Length of string */
fl_strlcat(char       *dst,     /* O - Destination string */
           const char *src,     /* I - Source string */
           size_t      size) {  /* I - Size of destination string buffer */
  size_t        srclen = strlen(src);
  size_t        dstlen = strlen(dst);

  if (dstlen >= size)
    return (size + srclen);

  size_t copylen = (srclen >= size - dstlen) ? (size - dstlen - 1) : srclen;
  memcpy(dst + dstlen, src, copylen);
  dst[dstlen + copylen] = '\0';

  return (dstlen + srclen);
}

#define C_RANGE(c,l,r) ( (c) >= (l) && (c) <= (r) )

/**
 * locale independent ascii oriented case cmp
 * returns 0 if string successfully compare, -1 if s<t, +1 if s>t
 */
int fl_ascii_strcasecmp(const char *s, const char *t) {
  if (!s || !t) return (s==t ? 0 : (!s ? -1 : +1));

  for(;*s && *t; s++,t++) {
    if (*s == *t) continue;
    if (*s < *t) {
      if ( (*s+0x20)!=*t || !C_RANGE(*s,'A','Z') ) return -1;
    } else {    /* (*s > *t) */
      if ( (*s-0x20)!=*t || !C_RANGE(*s,'a','z') ) return +1;
    }
  }
  return (*s==*t) ? 0 : (*t ? -1 : +1);
}
