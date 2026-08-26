//
// FLTK Benchmarks - Valuators, Sliders, Dials
//
#include "fltk_benchmarks.h"

#include <FL/Fl_Slider.H>
#include <FL/Fl_Fill_Slider.H>
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Hor_Fill_Slider.H>
#include <FL/Fl_Hor_Nice_Slider.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/Fl_Nice_Slider.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/Fl_Adjuster.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Simple_Counter.H>
#include <FL/Fl_Dial.H>
#include <FL/Fl_Fill_Dial.H>
#include <FL/Fl_Line_Dial.H>
#include <FL/Fl_Roller.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Value_Output.H>
#include <FL/Fl_Positioner.H>
#include <FL/Fl_Spinner.H>

using namespace fltk_bench;

static BenchmarkResult bench_Fl_Slider() {
  return benchmark_fltk_widget<Fl_Slider>("Fl_Slider", "Valuators", "FL/Fl_Slider.H",
    nullptr, [](Fl_Slider* s) { s->value(s->value() + 0.01); }, "slider_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Slider);

static BenchmarkResult bench_Fl_Fill_Slider() {
  return benchmark_fltk_widget<Fl_Fill_Slider>("Fl_Fill_Slider", "Valuators", "FL/Fl_Fill_Slider.H",
    nullptr, [](Fl_Fill_Slider* s) { s->value(s->value() + 0.01); }, "slider_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Fill_Slider);

static BenchmarkResult bench_Fl_Hor_Slider() {
  return benchmark_fltk_widget<Fl_Hor_Slider>("Fl_Hor_Slider", "Valuators", "FL/Fl_Hor_Slider.H",
    nullptr, [](Fl_Hor_Slider* s) { s->value(s->value() + 0.01); }, "slider_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Hor_Slider);

static BenchmarkResult bench_Fl_Hor_Fill_Slider() {
  return benchmark_fltk_widget<Fl_Hor_Fill_Slider>("Fl_Hor_Fill_Slider", "Valuators", "FL/Fl_Hor_Fill_Slider.H",
    nullptr, [](Fl_Hor_Fill_Slider* s) { s->value(s->value() + 0.01); }, "slider_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Hor_Fill_Slider);

static BenchmarkResult bench_Fl_Hor_Nice_Slider() {
  return benchmark_fltk_widget<Fl_Hor_Nice_Slider>("Fl_Hor_Nice_Slider", "Valuators", "FL/Fl_Hor_Nice_Slider.H",
    nullptr, [](Fl_Hor_Nice_Slider* s) { s->value(s->value() + 0.01); }, "slider_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Hor_Nice_Slider);

static BenchmarkResult bench_Fl_Hor_Value_Slider() {
  return benchmark_fltk_widget<Fl_Hor_Value_Slider>("Fl_Hor_Value_Slider", "Valuators", "FL/Fl_Hor_Value_Slider.H",
    nullptr, [](Fl_Hor_Value_Slider* s) { s->value(s->value() + 0.01); }, "slider_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Hor_Value_Slider);

static BenchmarkResult bench_Fl_Nice_Slider() {
  return benchmark_fltk_widget<Fl_Nice_Slider>("Fl_Nice_Slider", "Valuators", "FL/Fl_Nice_Slider.H",
    nullptr, [](Fl_Nice_Slider* s) { s->value(s->value() + 0.01); }, "slider_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Nice_Slider);

static BenchmarkResult bench_Fl_Value_Slider() {
  return benchmark_fltk_widget<Fl_Value_Slider>("Fl_Value_Slider", "Valuators", "FL/Fl_Value_Slider.H",
    nullptr, [](Fl_Value_Slider* s) { s->value(s->value() + 0.01); }, "slider_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Value_Slider);

static BenchmarkResult bench_Fl_Scrollbar() {
  return benchmark_fltk_widget<Fl_Scrollbar>("Fl_Scrollbar", "Valuators", "FL/Fl_Scrollbar.H",
    nullptr, [](Fl_Scrollbar* s) { s->value(s->value() + 1); }, "scrollbar_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Scrollbar);

static BenchmarkResult bench_Fl_Adjuster() {
  return benchmark_fltk_widget<Fl_Adjuster>("Fl_Adjuster", "Valuators", "FL/Fl_Adjuster.H",
    nullptr, [](Fl_Adjuster* a) { a->value(a->value() + 1.0); }, "adjuster_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Adjuster);

static BenchmarkResult bench_Fl_Counter() {
  return benchmark_fltk_widget<Fl_Counter>("Fl_Counter", "Valuators", "FL/Fl_Counter.H",
    nullptr, [](Fl_Counter* c) { c->value(c->value() + 1.0); }, "counter_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Counter);

static BenchmarkResult bench_Fl_Simple_Counter() {
  return benchmark_fltk_widget<Fl_Simple_Counter>("Fl_Simple_Counter", "Valuators", "FL/Fl_Simple_Counter.H",
    nullptr, [](Fl_Simple_Counter* c) { c->value(c->value() + 1.0); }, "simple_counter_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Simple_Counter);

static BenchmarkResult bench_Fl_Dial() {
  return benchmark_fltk_widget<Fl_Dial>("Fl_Dial", "Valuators", "FL/Fl_Dial.H",
    nullptr, [](Fl_Dial* d) { d->value(d->value() + 0.05); }, "dial_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Dial);

static BenchmarkResult bench_Fl_Fill_Dial() {
  return benchmark_fltk_widget<Fl_Fill_Dial>("Fl_Fill_Dial", "Valuators", "FL/Fl_Fill_Dial.H",
    nullptr, [](Fl_Fill_Dial* d) { d->value(d->value() + 0.05); }, "dial_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Fill_Dial);

static BenchmarkResult bench_Fl_Line_Dial() {
  return benchmark_fltk_widget<Fl_Line_Dial>("Fl_Line_Dial", "Valuators", "FL/Fl_Line_Dial.H",
    nullptr, [](Fl_Line_Dial* d) { d->value(d->value() + 0.05); }, "dial_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Line_Dial);

static BenchmarkResult bench_Fl_Roller() {
  return benchmark_fltk_widget<Fl_Roller>("Fl_Roller", "Valuators", "FL/Fl_Roller.H",
    nullptr, [](Fl_Roller* r) { r->value(r->value() + 0.05); }, "roller_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Roller);

static BenchmarkResult bench_Fl_Value_Input() {
  return benchmark_fltk_widget<Fl_Value_Input>("Fl_Value_Input", "Valuators", "FL/Fl_Value_Input.H",
    nullptr, [](Fl_Value_Input* v) { v->value(v->value() + 1.0); }, "val_input_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Value_Input);

static BenchmarkResult bench_Fl_Value_Output() {
  return benchmark_fltk_widget<Fl_Value_Output>("Fl_Value_Output", "Valuators", "FL/Fl_Value_Output.H",
    nullptr, [](Fl_Value_Output* v) { v->value(v->value() + 1.0); }, "val_output_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Value_Output);

static BenchmarkResult bench_Fl_Positioner() {
  return benchmark_fltk_widget<Fl_Positioner>("Fl_Positioner", "Valuators", "FL/Fl_Positioner.H",
    nullptr, [](Fl_Positioner* p) { p->xvalue(p->xvalue() + 0.01); p->yvalue(p->yvalue() + 0.01); }, "positioner_xy");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Positioner);

static BenchmarkResult bench_Fl_Spinner() {
  return benchmark_fltk_widget<Fl_Spinner>("Fl_Spinner", "Valuators", "FL/Fl_Spinner.H",
    nullptr, [](Fl_Spinner* s) { s->value(s->value() + 1.0); }, "spinner_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Spinner);
