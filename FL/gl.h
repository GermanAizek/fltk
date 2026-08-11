//
// OpenGL header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2018 by Bill Spitzak and others.
//
// You must include this instead of GL/gl.h to get the Microsoft
// APIENTRY stuff included (from <windows.h>) prior to the OpenGL
// header files.
//
// This file also provides "missing" OpenGL functions, and
// gl_start() and gl_finish() to allow OpenGL to be used in any window
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

/**
 \file gl.h
 This file defines wrapper functions for OpenGL in FLTK

 To use OpenGL from within an FLTK application you MUST use gl_visual()
 to select the default visual before doing show() on any windows. Mesa
 will crash if you try to use a visual not returned by glxChooseVisual.

 Historically, this did not always work well with Fl_Double_Window's!
 It can try to draw into the front buffer.
 Depending on the system this might either
 crash or do nothing (when pixmaps are being used as back buffer
 and GL is being done by hardware), work correctly (when GL is done
 with software, such as Mesa), or draw into the front buffer and
 be erased when the buffers are swapped (when double buffer hardware
 is being used)
 */

#ifndef FL_gl_H
#  define FL_gl_H

#  include "Enumerations.H" // for color names
#  ifdef _WIN32
#    include <windows.h>
#  endif
#  ifndef APIENTRY
#    if defined(__CYGWIN__)
#      define APIENTRY __attribute__ ((__stdcall__))
#    else
#      define APIENTRY
#    endif
#  endif

#  ifdef __APPLE__ // PORTME: OpenGL path abstraction
#    ifndef GL_SILENCE_DEPRECATION
#      define GL_SILENCE_DEPRECATION 1
#    endif
#    if !defined(__gl3_h_) // make sure OpenGL/gl3.h was not included before
#      include <OpenGL/gl.h>
#    endif
#  elif defined(__ANDROID__)
#    include <GLES/gl.h>
#  else
#    include <GL/gl.h>
#  endif  // __APPLE__ // PORTME: OpenGL Path abstraction

FL_EXPORT void gl_start();
FL_EXPORT void gl_finish();

FL_EXPORT void gl_color(Fl_Color i);
/** back compatibility */
inline void gl_color(int c) {gl_color((Fl_Color)c);}

FL_EXPORT void gl_rect(int x,int y,int w,int h);
FL_EXPORT void gl_rectf(int x,int y,int w,int h);

FL_EXPORT void gl_font(int fontid, int size);
FL_EXPORT int  gl_height();
FL_EXPORT int  gl_descent();
FL_EXPORT double gl_width(const char *);
FL_EXPORT double gl_width(const char *, int n);
FL_EXPORT double gl_width(uchar);

FL_EXPORT void gl_draw(const char*);
FL_EXPORT void gl_draw(const char*, int n);
FL_EXPORT void gl_draw(const char*, int x, int y);
FL_EXPORT void gl_draw(const char*, float x, float y);
FL_EXPORT void gl_draw(const char*, int n, int x, int y);
FL_EXPORT void gl_draw(const char*, int n, float x, float y);
FL_EXPORT void gl_draw(const char*, int x, int y, int w, int h, Fl_Align);
FL_EXPORT void gl_measure(const char*, int& x, int& y);
FL_EXPORT void gl_texture_pile_height(int max);
FL_EXPORT int  gl_texture_pile_height();
FL_EXPORT void gl_texture_reset();

FL_EXPORT void gl_draw_image(const uchar *, int x,int y,int w,int h, int d=3, int ld=0);

#ifdef __ANDROID__
// Mock desktop OpenGL functions missing in OpenGL ES to allow fltk_gl to compile
typedef float GLdouble;
#define glDrawBuffer(x) (void)0
#define glReadBuffer(x) (void)0
#define glOrtho glOrthof
#define glRasterPos2i(x, y) (void)0
#define glRasterPos2f(x, y) (void)0
#define glCopyPixels(x, y, w, h, t) (void)0
#define glPushAttrib(x) (void)0
#define glPopAttrib() (void)0
#define glPushClientAttrib(x) (void)0
#define glPopClientAttrib() (void)0
#define glDeleteLists(x, y) (void)0
#define glBegin(x) (void)0
#define glEnd() (void)0
#define glVertex2f(x, y) (void)0
#define glVertex2i(x, y) (void)0
#define glVertex3d(x, y, z) (void)0
#define glNormal3d(x, y, z) (void)0
#define glColor3ub(r, g, b) (void)0
#define glVertex3dv(v) (void)0
#define glNormal3dv(v) (void)0
#define glMap2d(...) (void)0
#define glMapGrid2d(...) (void)0
#define glEvalMesh2(...) (void)0
#define glRotated glRotatef
#define glScaled glScalef
#define glTranslated glTranslatef
#define glRecti(x,y,w,h) (void)0
#define glDrawPixels(w,h,f,t,p) (void)0
#define glTexCoord2f(x,y) (void)0
#define glCallLists(n,t,p) (void)0
#define glVertex2d(x,y) (void)0

#ifndef GL_INDEX_BITS
#define GL_INDEX_BITS 0
#endif
#ifndef GL_UNPACK_ROW_LENGTH
#define GL_UNPACK_ROW_LENGTH 0
#endif
#ifndef GL_CURRENT_RASTER_POSITION
#define GL_CURRENT_RASTER_POSITION 0
#endif
#ifndef GL_TEXTURE_WIDTH
#define GL_TEXTURE_WIDTH 0
#endif
#ifndef GL_TEXTURE_HEIGHT
#define GL_TEXTURE_HEIGHT 0
#endif
#ifndef GL_ALPHA8
#define GL_ALPHA8 0
#endif
#ifndef GL_CURRENT_RASTER_POSITION_VALID
#define GL_CURRENT_RASTER_POSITION_VALID 0
#endif

#ifndef GL_ALL_ATTRIB_BITS
#define GL_ALL_ATTRIB_BITS 0
#endif
#ifndef GL_CLIENT_PIXEL_STORE_BIT
#define GL_CLIENT_PIXEL_STORE_BIT 0
#endif
#ifndef GL_PACK_ROW_LENGTH
#define GL_PACK_ROW_LENGTH 0
#endif
#ifndef GL_PACK_SKIP_ROWS
#define GL_PACK_SKIP_ROWS 0
#endif
#ifndef GL_PACK_SKIP_PIXELS
#define GL_PACK_SKIP_PIXELS 0
#endif
#ifndef GL_COLOR
#define GL_COLOR 0
#endif
#ifndef GL_FRONT
#define GL_FRONT 0
#endif
#ifndef GL_BACK
#define GL_BACK 0
#endif
#ifndef GL_ENABLE_BIT
#define GL_ENABLE_BIT 0
#endif
#ifndef GL_EVAL_BIT
#define GL_EVAL_BIT 0
#endif
#ifndef GL_AUTO_NORMAL
#define GL_AUTO_NORMAL 0
#endif
#ifndef GL_MAP2_VERTEX_3
#define GL_MAP2_VERTEX_3 0
#endif
#ifndef GL_MAP2_TEXTURE_COORD_2
#define GL_MAP2_TEXTURE_COORD_2 0
#endif
#ifndef GL_LINE
#define GL_LINE 0
#endif
#ifndef GL_FILL
#define GL_FILL 0
#endif
#ifndef GL_QUADS
#define GL_QUADS 0
#endif
#ifndef GL_POLYGON
#define GL_POLYGON 0
#endif
#ifndef GL_LINE_LOOP
#define GL_LINE_LOOP 0
#endif
#ifndef GL_TRIANGLES
#define GL_TRIANGLES 0
#endif
#ifndef GL_NORMALIZE
#define GL_NORMALIZE 0
#endif
#endif // __ANDROID__

#endif // !FL_gl_H
