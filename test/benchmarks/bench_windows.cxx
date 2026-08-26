//
// FLTK Benchmarks - Windows
//
#include "fltk_benchmarks.h"

#include <FL/Fl_Window.H>
#include <FL/Fl_Single_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Overlay_Window.H>
#include <FL/Fl_Menu_Window.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Box.H>

using namespace fltk_bench;

static BenchmarkResult bench_Fl_Window() {
  return benchmark_fltk_widget<Fl_Window>("Fl_Window", "Windows", "FL/Fl_Window.H",
    [](Fl_Window* win) {
      win->begin();
      new Fl_Box(10, 10, 100, 30, "Inside");
      win->end();
    },
    [](Fl_Window* win) {
      win->size(win->w() + 1, win->h() + 1);
      win->size(win->w() - 1, win->h() - 1);
    }, "window_size_calc");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Window);

static BenchmarkResult bench_Fl_Single_Window() {
  return benchmark_fltk_widget<Fl_Single_Window>("Fl_Single_Window", "Windows", "FL/Fl_Single_Window.H",
    nullptr,
    [](Fl_Single_Window* win) {
      win->size(win->w() + 1, win->h() + 1);
      win->size(win->w() - 1, win->h() - 1);
    }, "single_win_size");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Single_Window);

static BenchmarkResult bench_Fl_Double_Window() {
  return benchmark_fltk_widget<Fl_Double_Window>("Fl_Double_Window", "Windows", "FL/Fl_Double_Window.H",
    nullptr,
    [](Fl_Double_Window* win) {
      win->size(win->w() + 1, win->h() + 1);
      win->size(win->w() - 1, win->h() - 1);
    }, "double_win_size");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Double_Window);

class BenchOverlayWindow : public Fl_Overlay_Window {
public:
  BenchOverlayWindow(int x, int y, int w, int h, const char* l = 0) : Fl_Overlay_Window(x, y, w, h, l) {}
  void draw_overlay() override {}
};

static BenchmarkResult bench_Fl_Overlay_Window() {
  BenchmarkResult res = benchmark_fltk_widget<BenchOverlayWindow>("Fl_Overlay_Window", "Windows", "FL/Fl_Overlay_Window.H",
    nullptr,
    [](BenchOverlayWindow* win) {
      win->size(win->w() + 1, win->h() + 1);
      win->size(win->w() - 1, win->h() - 1);
    }, "overlay_win_size");
  res.class_name = "Fl_Overlay_Window";
  return res;
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Overlay_Window);

static BenchmarkResult bench_Fl_Menu_Window() {
  return benchmark_fltk_widget<Fl_Menu_Window>("Fl_Menu_Window", "Windows", "FL/Fl_Menu_Window.H",
    nullptr,
    [](Fl_Menu_Window* win) {
      win->size(win->w() + 1, win->h() + 1);
      win->size(win->w() - 1, win->h() - 1);
    }, "menu_win_size");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Menu_Window);

#if defined(FLTK_USE_GL) || defined(FL_GL_H) || defined(FL_DOXYGEN) || 1
static BenchmarkResult bench_Fl_Gl_Window() {
  return benchmark_fltk_class<Fl_Gl_Window>("Fl_Gl_Window", "Windows", "FL/Fl_Gl_Window.H",
    []() { return new Fl_Gl_Window(0, 0, 100, 100); },
    nullptr, "gl_window_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Gl_Window);
#endif
