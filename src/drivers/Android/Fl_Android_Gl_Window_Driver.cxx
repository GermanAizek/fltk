//
// Class Fl_Android_Gl_Window_Driver for the Fast Light Tool Kit (FLTK).
//
// Copyright 2026 Hermann Semenoff (GermanAizek)
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
#if HAVE_GL
#include <FL/platform.H>
#include <FL/Fl_Image_Surface.H>
#include "../../Fl_Gl_Choice.H"
#include "Fl_Android_Window_Driver.H"
#include "Fl_Android_Graphics_Driver.H"
#include "Fl_Android_Gl_Window_Driver.H"
#include "Fl_Android_Application.H"
#include "../Posix/Fl_Posix_System_Driver.H"
#include <EGL/egl.h>
#include <GLES/gl.h>

class Fl_Android_Gl_Choice : public Fl_Gl_Choice {
  friend class Fl_Android_Gl_Window_Driver;
private:
  EGLConfig egl_conf;
public:
  Fl_Android_Gl_Choice(int m, const int *alistp, Fl_Gl_Choice *n) : Fl_Gl_Choice(m, alistp, n) {
  egl_conf = 0;
  }
};

EGLDisplay Fl_Android_Gl_Window_Driver::egl_display = NULL;

Fl_Gl_Window_Driver* Fl_Gl_Window_Driver::newGlWindowDriver(Fl_Gl_Window *w) {
  return new Fl_Android_Gl_Window_Driver(w);
}

Fl_Android_Gl_Window_Driver::Fl_Android_Gl_Window_Driver(Fl_Gl_Window *w) : Fl_Gl_Window_Driver(w) {
  egl_surface = NULL;
}

float Fl_Android_Gl_Window_Driver::pixels_per_unit() {
  return Fl::screen_scale(pWindow->screen_num());
}

void Fl_Android_Gl_Window_Driver::init() {
  if (egl_display) return;
  EGLint major, minor;
  egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (!egl_display) {
    Fl::fatal("Can't create egl display\n");
  }
  if (eglInitialize(egl_display, &major, &minor) != EGL_TRUE) {
    Fl::fatal("Can't initialise egl display\n");
  }
  eglBindAPI(EGL_OPENGL_ES_API);
}

Fl_Gl_Choice *Fl_Android_Gl_Window_Driver::find(int m, const int *alistp)
{
  init();
  m |= FL_DOUBLE;
  Fl_Android_Gl_Choice *g = (Fl_Android_Gl_Choice*)Fl_Gl_Window_Driver::find_begin(m, alistp);
  if (g) return g;

  EGLint n;
  EGLint config_attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
    EGL_DEPTH_SIZE, 0, // set at 11
    EGL_SAMPLE_BUFFERS, 0,  // set at 13
    EGL_STENCIL_SIZE, 0, // set at 15
    EGL_ALPHA_SIZE, 0, // set at 17
    EGL_NONE
  };

  if (m & FL_DEPTH32) config_attribs[11] = 32;
  else if (m & FL_DEPTH) config_attribs[11] = 16;
  if (m & FL_MULTISAMPLE) config_attribs[13] = 1;
  if (m & FL_STENCIL) config_attribs[15] = 1;
  if (m & FL_ALPHA) config_attribs[17] = (m & FL_RGB8) ? 8 : 1;

  g = new Fl_Android_Gl_Choice(m, alistp, first);
  eglChooseConfig(egl_display, config_attribs, &(g->egl_conf), 1, &n);
  if (n == 0 && (m & FL_MULTISAMPLE)) {
    config_attribs[13] = 0;
    eglChooseConfig(egl_display, config_attribs, &(g->egl_conf), 1, &n);
  }
  if (n == 0) {
    Fl::fatal("failed to choose an EGL config for Android\n");
  }
  first = g;
  return g;
}

GLContext Fl_Android_Gl_Window_Driver::create_gl_context(Fl_Window* window, const Fl_Gl_Choice* g) {
  GLContext shared_ctx = 0;
  if (context_list && nContext) shared_ctx = context_list[0];

  static const EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 1, EGL_NONE };
  GLContext ctx = (GLContext)eglCreateContext(egl_display,
    ((Fl_Android_Gl_Choice*)g)->egl_conf,
    (shared_ctx ? (EGLContext)shared_ctx : EGL_NO_CONTEXT),
    context_attribs);
  if (ctx) {
    add_context(ctx);
  }
  return ctx;
}

void Fl_Android_Gl_Window_Driver::set_gl_context(Fl_Window* w, GLContext context) {
  ANativeWindow* win = Fl_Android_Application::native_window();
  if (!win) return;
  if (!egl_surface) {
    Fl_Android_Gl_Choice* g = (Fl_Android_Gl_Choice*)Fl_Gl_Window_Driver::g();
    if (g && g->egl_conf) {
      egl_surface = eglCreateWindowSurface(egl_display, g->egl_conf, win, NULL);
    }
  }
  GLContext current_context = eglGetCurrentContext();
  if (context != current_context || w != cached_window) {
    cached_window = w;
    if (eglMakeCurrent(egl_display, egl_surface, egl_surface, (EGLContext)context)) {
    } else {
      Fl::error("eglMakeCurrent() failed\n");
    }
  }
}

void Fl_Android_Gl_Window_Driver::delete_gl_context(GLContext context) {
  GLContext current_context = eglGetCurrentContext();
  if (current_context == context) {
    cached_window = 0;
  }
  if (current_context == (EGLContext)context) {
    eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  }
  eglDestroyContext(egl_display, (EGLContext)context);
  del_context(context);
}

void Fl_Android_Gl_Window_Driver::make_current_before() {
  if (!egl_display) init();
  if (!pWindow->g) {
    pWindow->g = find(pWindow->mode_, pWindow->alist);
    pWindow->context(create_gl_context(pWindow, pWindow->g), 1);
    pWindow->valid(0);
  }
  set_gl_context(pWindow, pWindow->context());
}

int Fl_Android_Gl_Window_Driver::mode_(int m, const int *a) {
  Fl_Gl_Choice *g = find(m, a);
  if (!g) return 0;
  if (egl_surface) {
    eglDestroySurface(egl_display, egl_surface);
    egl_surface = NULL;
  }
  return 1;
}

void Fl_Android_Gl_Window_Driver::swap_buffers() {
  if (egl_surface) {
    eglSwapBuffers(egl_display, egl_surface);
  }
}

void Fl_Android_Gl_Window_Driver::resize(int is_a_resize, int w, int h) {
}

char Fl_Android_Gl_Window_Driver::swap_type() {
  return 1;
}

void Fl_Android_Gl_Window_Driver::swap_interval(int) {}
int Fl_Android_Gl_Window_Driver::swap_interval() const { return -1; }

void Fl_Android_Gl_Window_Driver::make_overlay_current() {}
void Fl_Android_Gl_Window_Driver::redraw_overlay() {}

void Fl_Android_Gl_Window_Driver::gl_start() {}
void Fl_Android_Gl_Window_Driver::gl_visual(Fl_Gl_Choice *c) {}

void* Fl_Android_Gl_Window_Driver::GetProcAddress(const char *procName) {
  return (void*)eglGetProcAddress(procName);
}

#endif // HAVE_GL
