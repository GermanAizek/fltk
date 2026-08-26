//
// FLTK Benchmarks - Buttons and Toggles
//
#include "fltk_benchmarks.h"

#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Radio_Button.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Radio_Light_Button.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Toggle_Light_Button.H>
#include <FL/Fl_Toggle_Round_Button.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Shortcut_Button.H>

using namespace fltk_bench;

static BenchmarkResult bench_Fl_Button() {
  return benchmark_fltk_widget<Fl_Button>("Fl_Button", "Buttons", "FL/Fl_Button.H",
    [](Fl_Button* b) { b->value(0); },
    [](Fl_Button* b) { b->value(!b->value()); }, "value_toggle");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Button);

static BenchmarkResult bench_Fl_Check_Button() {
  return benchmark_fltk_widget<Fl_Check_Button>("Fl_Check_Button", "Buttons", "FL/Fl_Check_Button.H",
    nullptr, [](Fl_Check_Button* b) { b->value(!b->value()); }, "check_toggle");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Check_Button);

static BenchmarkResult bench_Fl_Radio_Button() {
  return benchmark_fltk_widget<Fl_Radio_Button>("Fl_Radio_Button", "Buttons", "FL/Fl_Radio_Button.H",
    nullptr, [](Fl_Radio_Button* b) { b->setonly(); }, "radio_setonly");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Radio_Button);

static BenchmarkResult bench_Fl_Round_Button() {
  return benchmark_fltk_widget<Fl_Round_Button>("Fl_Round_Button", "Buttons", "FL/Fl_Round_Button.H",
    nullptr, [](Fl_Round_Button* b) { b->value(!b->value()); }, "round_toggle");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Round_Button);

static BenchmarkResult bench_Fl_Light_Button() {
  return benchmark_fltk_widget<Fl_Light_Button>("Fl_Light_Button", "Buttons", "FL/Fl_Light_Button.H",
    nullptr, [](Fl_Light_Button* b) { b->value(!b->value()); }, "light_toggle");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Light_Button);

static BenchmarkResult bench_Fl_Radio_Light_Button() {
  return benchmark_fltk_widget<Fl_Radio_Light_Button>("Fl_Radio_Light_Button", "Buttons", "FL/Fl_Radio_Light_Button.H",
    nullptr, [](Fl_Radio_Light_Button* b) { b->setonly(); }, "radio_light_setonly");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Radio_Light_Button);

static BenchmarkResult bench_Fl_Radio_Round_Button() {
  return benchmark_fltk_widget<Fl_Radio_Round_Button>("Fl_Radio_Round_Button", "Buttons", "FL/Fl_Radio_Round_Button.H",
    nullptr, [](Fl_Radio_Round_Button* b) { b->setonly(); }, "radio_round_setonly");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Radio_Round_Button);

static BenchmarkResult bench_Fl_Toggle_Button() {
  return benchmark_fltk_widget<Fl_Toggle_Button>("Fl_Toggle_Button", "Buttons", "FL/Fl_Toggle_Button.H",
    nullptr, [](Fl_Toggle_Button* b) { b->value(!b->value()); }, "toggle_val");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Toggle_Button);

static BenchmarkResult bench_Fl_Toggle_Light_Button() {
  return benchmark_fltk_widget<Fl_Toggle_Light_Button>("Fl_Toggle_Light_Button", "Buttons", "FL/Fl_Toggle_Light_Button.H",
    nullptr, [](Fl_Toggle_Light_Button* b) { b->value(!b->value()); }, "toggle_light_val");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Toggle_Light_Button);

static BenchmarkResult bench_Fl_Toggle_Round_Button() {
  return benchmark_fltk_widget<Fl_Toggle_Round_Button>("Fl_Toggle_Round_Button", "Buttons", "FL/Fl_Toggle_Round_Button.H",
    nullptr, [](Fl_Toggle_Round_Button* b) { b->value(!b->value()); }, "toggle_round_val");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Toggle_Round_Button);

static BenchmarkResult bench_Fl_Repeat_Button() {
  return benchmark_fltk_widget<Fl_Repeat_Button>("Fl_Repeat_Button", "Buttons", "FL/Fl_Repeat_Button.H",
    nullptr, [](Fl_Repeat_Button* b) { b->value(!b->value()); }, "repeat_btn_val");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Repeat_Button);

static BenchmarkResult bench_Fl_Return_Button() {
  return benchmark_fltk_widget<Fl_Return_Button>("Fl_Return_Button", "Buttons", "FL/Fl_Return_Button.H",
    nullptr, [](Fl_Return_Button* b) { b->value(!b->value()); }, "return_btn_val");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Return_Button);

static BenchmarkResult bench_Fl_Shortcut_Button() {
  return benchmark_fltk_widget<Fl_Shortcut_Button>("Fl_Shortcut_Button", "Buttons", "FL/Fl_Shortcut_Button.H",
    nullptr, [](Fl_Shortcut_Button* b) { b->shortcut(FL_CTRL + 'a'); }, "set_shortcut");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Shortcut_Button);
