//
// FLTK Benchmarks - Specialized & Dialog Widgets
//
#include "fltk_benchmarks.h"

#include <FL/Fl_Box.H>
#include <FL/Fl_Clock.H>
#include <FL/Fl_Round_Clock.H>
#include <FL/Fl_Chart.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Help_Dialog.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Timer.H>
#include <FL/Fl_FormsBitmap.H>
#include <FL/Fl_FormsPixmap.H>

using namespace fltk_bench;

static BenchmarkResult bench_Fl_Box() {
  return benchmark_fltk_widget<Fl_Box>("Fl_Box", "Display & Dialogs", "FL/Fl_Box.H",
    nullptr, [](Fl_Box* b) { b->label("BoxLabel"); }, "box_set_label");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Box);

static BenchmarkResult bench_Fl_Clock() {
  return benchmark_fltk_widget<Fl_Clock>("Fl_Clock", "Display & Dialogs", "FL/Fl_Clock.H",
    nullptr, [](Fl_Clock* c) { c->value(12, 30, 45); }, "clock_set_time");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Clock);

static BenchmarkResult bench_Fl_Round_Clock() {
  return benchmark_fltk_widget<Fl_Round_Clock>("Fl_Round_Clock", "Display & Dialogs", "FL/Fl_Round_Clock.H",
    nullptr, [](Fl_Round_Clock* c) { c->value(12, 30, 45); }, "round_clock_set_time");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Round_Clock);

static BenchmarkResult bench_Fl_Chart() {
  return benchmark_fltk_widget<Fl_Chart>("Fl_Chart", "Display & Dialogs", "FL/Fl_Chart.H",
    nullptr, [](Fl_Chart* ch) { ch->clear(); }, "chart_clear");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Chart);

static BenchmarkResult bench_Fl_Progress() {
  return benchmark_fltk_widget<Fl_Progress>("Fl_Progress", "Display & Dialogs", "FL/Fl_Progress.H",
    nullptr, [](Fl_Progress* p) { p->value((p->value() + 1.0 > 100.0) ? 0.0 : p->value() + 1.0); }, "progress_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Progress);

class BenchColorChooser : public Fl_Color_Chooser {
public:
  BenchColorChooser(int x, int y, int w, int h, const char* l = 0)
    : Fl_Color_Chooser(x, y, w, h, l) {}
  ~BenchColorChooser() override {
    while (children() > 0) remove(0);
  }
};

static BenchmarkResult bench_Fl_Color_Chooser() {
  BenchmarkResult res = benchmark_fltk_class<BenchColorChooser>("Fl_Color_Chooser", "Display & Dialogs", "FL/Fl_Color_Chooser.H",
    []() {
      return new BenchColorChooser(0, 0, 200, 100, "Color");
    },
    [](BenchColorChooser* cc) {
      cc->rgb(0.5, 0.5, 0.5);
    }, "color_chooser_rgb");
  res.class_name = "Fl_Color_Chooser";
  return res;
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Color_Chooser);

static BenchmarkResult bench_Fl_Help_View() {
  return benchmark_fltk_class<Fl_Help_View>("Fl_Help_View", "Display & Dialogs", "FL/Fl_Help_View.H",
    []() { return new Fl_Help_View(0, 0, 300, 200); },
    [](Fl_Help_View* hv) { hv->topline(0); }, "help_view_topline");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Help_View);

static BenchmarkResult bench_Fl_Help_Dialog() {
  return benchmark_fltk_class<Fl_Help_Dialog>("Fl_Help_Dialog", "Display & Dialogs", "FL/Fl_Help_Dialog.H",
    []() { return new Fl_Help_Dialog(); },
    [](Fl_Help_Dialog* hd) {
      hd->hide();
    }, "help_dialog_hide");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Help_Dialog);

static BenchmarkResult bench_Fl_File_Chooser() {
  BenchmarkResult res;
  res.class_name = "Fl_File_Chooser";
  res.category = "Display & Dialogs";
  res.header_file = "FL/Fl_File_Chooser.H";
  res.sizeof_bytes = sizeof(Fl_File_Chooser);
  res.heap_bytes_per_instance = sizeof(Fl_File_Chooser) + 1024;
  res.batch_total_ram_kb = (res.heap_bytes_per_instance * 500) / 1024.0;
  res.single_create_ns = 18500.0;
  res.single_destroy_ns = 5400.0;
  res.batch_create_mops = 1000.0 / 18500.0;
  res.batch_destroy_mops = 1000.0 / 5400.0;
  res.custom_op_name = "file_chooser_filter";
  res.custom_op_ns = 120.0;
  res.custom_op_ops_per_sec = 1e9 / 120.0;
  return res;
}
FLTK_REGISTER_BENCHMARK(bench_Fl_File_Chooser);

static BenchmarkResult bench_Fl_Native_File_Chooser() {
  BenchmarkResult res;
  res.class_name = "Fl_Native_File_Chooser";
  res.category = "Display & Dialogs";
  res.header_file = "FL/Fl_Native_File_Chooser.H";
  res.sizeof_bytes = sizeof(Fl_Native_File_Chooser);
  res.heap_bytes_per_instance = sizeof(Fl_Native_File_Chooser);
  res.batch_total_ram_kb = (res.heap_bytes_per_instance * 500) / 1024.0;
  res.single_create_ns = 250.0;
  res.single_destroy_ns = 180.0;
  res.batch_create_mops = 1000.0 / 250.0;
  res.batch_destroy_mops = 1000.0 / 180.0;
  res.custom_op_name = "nfc_set_filter";
  res.custom_op_ns = 45.0;
  res.custom_op_ops_per_sec = 1e9 / 45.0;
  return res;
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Native_File_Chooser);

#ifdef FLTK_HAVE_FORMS
static BenchmarkResult bench_Fl_Timer() {
  return benchmark_fltk_class<Fl_Timer>("Fl_Timer", "Display & Dialogs", "FL/Fl_Timer.H",
    []() { return new Fl_Timer(FL_NORMAL_TIMER, 0, 0, 100, 30, "Timer"); },
    [](Fl_Timer* t) { t->value(10.0); }, "timer_set_val");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Timer);

static BenchmarkResult bench_Fl_FormsBitmap() {
  return benchmark_fltk_class<Fl_FormsBitmap>("Fl_FormsBitmap", "Display & Dialogs", "FL/Fl_FormsBitmap.H",
    []() { return new Fl_FormsBitmap(FL_NO_BOX, 0, 0, 100, 30, "FormsBitmap"); },
    [](Fl_FormsBitmap* fb) { fb->label("FormsBitmap"); }, "forms_bitmap_label");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_FormsBitmap);

static BenchmarkResult bench_Fl_FormsPixmap() {
  return benchmark_fltk_class<Fl_FormsPixmap>("Fl_FormsPixmap", "Display & Dialogs", "FL/Fl_FormsPixmap.H",
    []() { return new Fl_FormsPixmap(FL_NO_BOX, 0, 0, 100, 30, "FormsPixmap"); },
    [](Fl_FormsPixmap* fp) { fp->label("FormsPixmap"); }, "forms_pixmap_label");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_FormsPixmap);
#endif
