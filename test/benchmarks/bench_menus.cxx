//
// FLTK Benchmarks - Menus and Choices
//
#include "fltk_benchmarks.h"

#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Sys_Menu_Bar.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Input_Choice.H>
#include <FL/Fl_Scheme_Choice.H>
#include <FL/Fl_Menu_Item.H>

using namespace fltk_bench;

static BenchmarkResult bench_Fl_Menu_Bar() {
  return benchmark_fltk_widget<Fl_Menu_Bar>("Fl_Menu_Bar", "Menus & Choices", "FL/Fl_Menu_Bar.H",
    [](Fl_Menu_Bar* mb) {
      mb->add("File/New", FL_CTRL+'n', 0);
      mb->add("File/Open", FL_CTRL+'o', 0);
      mb->add("File/Save", FL_CTRL+'s', 0);
      mb->add("Edit/Copy", FL_CTRL+'c', 0);
      mb->add("Edit/Paste", FL_CTRL+'v', 0);
      mb->add("Help/About", 0, 0);
    },
    [](Fl_Menu_Bar* mb) {
      mb->find_item("File/Open");
    }, "find_menu_item");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Menu_Bar);

static BenchmarkResult bench_Fl_Sys_Menu_Bar() {
  return benchmark_fltk_widget<Fl_Sys_Menu_Bar>("Fl_Sys_Menu_Bar", "Menus & Choices", "FL/Fl_Sys_Menu_Bar.H",
    [](Fl_Sys_Menu_Bar* mb) {
      mb->add("File/New", FL_CTRL+'n', 0);
      mb->add("File/Open", FL_CTRL+'o', 0);
      mb->add("Edit/Copy", FL_CTRL+'c', 0);
    },
    [](Fl_Sys_Menu_Bar* mb) {
      mb->find_item("File/Open");
    }, "find_sys_menu");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Sys_Menu_Bar);

static BenchmarkResult bench_Fl_Menu_Button() {
  return benchmark_fltk_widget<Fl_Menu_Button>("Fl_Menu_Button", "Menus & Choices", "FL/Fl_Menu_Button.H",
    [](Fl_Menu_Button* mb) {
      mb->add("Item 1");
      mb->add("Item 2");
      mb->add("Item 3");
    },
    [](Fl_Menu_Button* mb) {
      mb->value(1);
    }, "menu_btn_val");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Menu_Button);

static BenchmarkResult bench_Fl_Choice() {
  return benchmark_fltk_widget<Fl_Choice>("Fl_Choice", "Menus & Choices", "FL/Fl_Choice.H",
    [](Fl_Choice* c) {
      c->add("Option A");
      c->add("Option B");
      c->add("Option C");
    },
    [](Fl_Choice* c) {
      c->value((c->value() + 1) % 3);
    }, "choice_step");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Choice);

static BenchmarkResult bench_Fl_Input_Choice() {
  return benchmark_fltk_widget<Fl_Input_Choice>("Fl_Input_Choice", "Menus & Choices", "FL/Fl_Input_Choice.H",
    [](Fl_Input_Choice* ic) {
      ic->add("Choice 1");
      ic->add("Choice 2");
      ic->add("Choice 3");
    },
    [](Fl_Input_Choice* ic) {
      ic->value("Choice 2");
    }, "input_choice_val");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Input_Choice);

static BenchmarkResult bench_Fl_Scheme_Choice() {
  return benchmark_fltk_widget<Fl_Scheme_Choice>("Fl_Scheme_Choice", "Menus & Choices", "FL/Fl_Scheme_Choice.H",
    nullptr,
    [](Fl_Scheme_Choice* sc) {
      sc->value(0);
    }, "scheme_choice_val");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Scheme_Choice);

static BenchmarkResult bench_Fl_Menu_Item() {
  return benchmark_fltk_class<Fl_Menu_Item>("Fl_Menu_Item", "Menus & Choices", "FL/Fl_Menu_Item.H",
    []() {
      Fl_Menu_Item* m = new Fl_Menu_Item();
      m->text = "Sample Item";
      m->shortcut_ = FL_CTRL + 's';
      return m;
    },
    [](Fl_Menu_Item* m) {
      m->activate();
      m->deactivate();
    }, "item_activate");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Menu_Item);
