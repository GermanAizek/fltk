//
// FLTK Benchmarks - Collections, Browsers, Trees, Tables
//
#include "fltk_benchmarks.h"

#include <FL/Fl_Browser.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Multi_Browser.H>
#include <FL/Fl_Check_Browser.H>
#include <FL/Fl_File_Browser.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Table_Row.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Tree_Item.H>
#include <FL/Fl_Tree_Prefs.H>

using namespace fltk_bench;

static BenchmarkResult bench_Fl_Browser() {
  return benchmark_fltk_widget<Fl_Browser>("Fl_Browser", "Browsers & Tables", "FL/Fl_Browser.H",
    nullptr, [](Fl_Browser* b) { b->clear(); }, "browser_clear");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Browser);

static BenchmarkResult bench_Fl_Hold_Browser() {
  return benchmark_fltk_widget<Fl_Hold_Browser>("Fl_Hold_Browser", "Browsers & Tables", "FL/Fl_Hold_Browser.H",
    nullptr, [](Fl_Hold_Browser* b) { b->clear(); }, "hold_browser_clear");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Hold_Browser);

static BenchmarkResult bench_Fl_Select_Browser() {
  return benchmark_fltk_widget<Fl_Select_Browser>("Fl_Select_Browser", "Browsers & Tables", "FL/Fl_Select_Browser.H",
    nullptr, [](Fl_Select_Browser* b) { b->clear(); }, "select_browser_clear");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Select_Browser);

static BenchmarkResult bench_Fl_Multi_Browser() {
  return benchmark_fltk_widget<Fl_Multi_Browser>("Fl_Multi_Browser", "Browsers & Tables", "FL/Fl_Multi_Browser.H",
    nullptr, [](Fl_Multi_Browser* b) { b->clear(); }, "multi_browser_clear");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Multi_Browser);

static BenchmarkResult bench_Fl_Check_Browser() {
  return benchmark_fltk_widget<Fl_Check_Browser>("Fl_Check_Browser", "Browsers & Tables", "FL/Fl_Check_Browser.H",
    nullptr, [](Fl_Check_Browser* b) { b->clear(); }, "check_browser_clear");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Check_Browser);

static BenchmarkResult bench_Fl_File_Browser() {
  return benchmark_fltk_widget<Fl_File_Browser>("Fl_File_Browser", "Browsers & Tables", "FL/Fl_File_Browser.H",
    nullptr, [](Fl_File_Browser* b) { b->clear(); }, "file_browser_clear");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_File_Browser);

static BenchmarkResult bench_Fl_Table() {
  return benchmark_fltk_widget<Fl_Table>("Fl_Table", "Browsers & Tables", "FL/Fl_Table.H",
    [](Fl_Table* t) {
      t->rows(100);
      t->cols(20);
    },
    [](Fl_Table* t) {
      t->row_height(5, 30);
      t->col_width(5, 80);
    }, "table_resize_cells");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Table);

static BenchmarkResult bench_Fl_Table_Row() {
  return benchmark_fltk_widget<Fl_Table_Row>("Fl_Table_Row", "Browsers & Tables", "FL/Fl_Table_Row.H",
    [](Fl_Table_Row* t) {
      t->rows(100);
      t->cols(20);
    },
    [](Fl_Table_Row* t) {
      t->select_row(10, 1);
    }, "table_row_select");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Table_Row);

static BenchmarkResult bench_Fl_Tree() {
  return benchmark_fltk_widget<Fl_Tree>("Fl_Tree", "Trees", "FL/Fl_Tree.H",
    nullptr, [](Fl_Tree* t) { t->clear(); }, "tree_clear");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Tree);

static BenchmarkResult bench_Fl_Tree_Item() {
  static Fl_Tree tree(0, 0, 100, 100);
  return benchmark_fltk_class<Fl_Tree_Item>("Fl_Tree_Item", "Trees", "FL/Fl_Tree_Item.H",
    []() {
      return new Fl_Tree_Item(&tree);
    },
    [](Fl_Tree_Item* item) {
      item->open();
      item->close();
    }, "tree_item_toggle");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Tree_Item);

static BenchmarkResult bench_Fl_Tree_Prefs() {
  return benchmark_fltk_class<Fl_Tree_Prefs>("Fl_Tree_Prefs", "Trees", "FL/Fl_Tree_Prefs.H",
    []() { return new Fl_Tree_Prefs(); },
    [](Fl_Tree_Prefs* p) {
      p->item_labelfont(FL_HELVETICA);
      p->item_labelsize(14);
    }, "tree_prefs_set");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Tree_Prefs);
