//
// FLTK Benchmarks - Containers and Layout
//
#include "fltk_benchmarks.h"

#include <FL/Fl_Group.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Grid.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Wizard.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Box.H>

using namespace fltk_bench;

static BenchmarkResult bench_Fl_Group() {
  return benchmark_fltk_widget<Fl_Group>("Fl_Group", "Containers & Layout", "FL/Fl_Group.H",
    [](Fl_Group* g) {
      g->begin();
      for (int i = 0; i < 10; ++i) {
        new Fl_Box(0, 0, 20, 20);
      }
      g->end();
    },
    [](Fl_Group* g) {
      g->find(g->child(5));
    }, "group_find_child");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Group);

static BenchmarkResult bench_Fl_Pack() {
  return benchmark_fltk_widget<Fl_Pack>("Fl_Pack", "Containers & Layout", "FL/Fl_Pack.H",
    [](Fl_Pack* p) {
      p->begin();
      for (int i = 0; i < 10; ++i) {
        new Fl_Box(0, 0, 20, 20);
      }
      p->end();
    },
    [](Fl_Pack* p) {
      p->resize(0, 0, p->w() + 1, p->h() + 1);
      p->resize(0, 0, p->w() - 1, p->h() - 1);
    }, "pack_layout_resize");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Pack);

static BenchmarkResult bench_Fl_Flex() {
  return benchmark_fltk_widget<Fl_Flex>("Fl_Flex", "Containers & Layout", "FL/Fl_Flex.H",
    [](Fl_Flex* f) {
      f->begin();
      for (int i = 0; i < 5; ++i) {
        new Fl_Box(0, 0, 20, 20);
      }
      f->end();
    },
    [](Fl_Flex* f) {
      f->layout();
    }, "flex_layout_calc");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Flex);

static BenchmarkResult bench_Fl_Grid() {
  return benchmark_fltk_widget<Fl_Grid>("Fl_Grid", "Containers & Layout", "FL/Fl_Grid.H",
    [](Fl_Grid* g) {
      g->layout(4, 4);
      for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
          Fl_Box* b = new Fl_Box(0, 0, 20, 20);
          g->widget(b, r, c);
        }
      }
    },
    [](Fl_Grid* g) {
      g->layout();
    }, "grid_layout_calc");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Grid);

static BenchmarkResult bench_Fl_Tabs() {
  return benchmark_fltk_widget<Fl_Tabs>("Fl_Tabs", "Containers & Layout", "FL/Fl_Tabs.H",
    [](Fl_Tabs* t) {
      t->begin();
      for (int i = 0; i < 3; ++i) {
        Fl_Group* g = new Fl_Group(10, 30, 200, 150, "Tab");
        g->end();
      }
      t->end();
    },
    [](Fl_Tabs* t) {
      t->value(t->child(0));
    }, "tabs_switch");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Tabs);

static BenchmarkResult bench_Fl_Tile() {
  return benchmark_fltk_widget<Fl_Tile>("Fl_Tile", "Containers & Layout", "FL/Fl_Tile.H",
    [](Fl_Tile* t) {
      t->begin();
      new Fl_Box(0, 0, 100, 100);
      new Fl_Box(100, 0, 100, 100);
      t->end();
    },
    [](Fl_Tile* t) {
      t->move_intersection(100, 100, 110, 100);
      t->move_intersection(110, 100, 100, 100);
    }, "tile_reposition");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Tile);

static BenchmarkResult bench_Fl_Wizard() {
  return benchmark_fltk_widget<Fl_Wizard>("Fl_Wizard", "Containers & Layout", "FL/Fl_Wizard.H",
    [](Fl_Wizard* w) {
      w->begin();
      new Fl_Group(0, 0, 200, 200, "Page 1");
      new Fl_Group(0, 0, 200, 200, "Page 2");
      w->end();
    },
    [](Fl_Wizard* w) {
      w->next();
    }, "wizard_next_page");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Wizard);

static BenchmarkResult bench_Fl_Scroll() {
  return benchmark_fltk_widget<Fl_Scroll>("Fl_Scroll", "Containers & Layout", "FL/Fl_Scroll.H",
    [](Fl_Scroll* s) {
      s->begin();
      for (int i = 0; i < 20; ++i) {
        new Fl_Box(0, i * 30, 200, 25);
      }
      s->end();
    },
    [](Fl_Scroll* s) {
      s->scroll_to(0, (s->yposition() + 10) % 200);
    }, "scroll_to_pos");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Scroll);
