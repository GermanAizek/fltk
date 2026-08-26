//
// FLTK Benchmarks - Text and Input
//
#include "fltk_benchmarks.h"

#include <FL/Fl_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_File_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Terminal.H>

using namespace fltk_bench;

static BenchmarkResult bench_Fl_Input() {
  return benchmark_fltk_widget<Fl_Input>("Fl_Input", "Text & Editors", "FL/Fl_Input.H",
    [](Fl_Input* in) { in->value("Hello FLTK"); },
    [](Fl_Input* in) { in->value("Quick brown fox jumps over lazy dog"); }, "set_value");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Input);

static BenchmarkResult bench_Fl_Float_Input() {
  return benchmark_fltk_widget<Fl_Float_Input>("Fl_Float_Input", "Text & Editors", "FL/Fl_Float_Input.H",
    nullptr, [](Fl_Float_Input* in) { in->value("3.14159265"); }, "set_float");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Float_Input);

static BenchmarkResult bench_Fl_Int_Input() {
  return benchmark_fltk_widget<Fl_Int_Input>("Fl_Int_Input", "Text & Editors", "FL/Fl_Int_Input.H",
    nullptr, [](Fl_Int_Input* in) { in->value("123456789"); }, "set_int");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Int_Input);

static BenchmarkResult bench_Fl_Multiline_Input() {
  return benchmark_fltk_widget<Fl_Multiline_Input>("Fl_Multiline_Input", "Text & Editors", "FL/Fl_Multiline_Input.H",
    nullptr, [](Fl_Multiline_Input* in) { in->value("Line 1\nLine 2\nLine 3\n"); }, "set_multiline");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Multiline_Input);

static BenchmarkResult bench_Fl_Secret_Input() {
  return benchmark_fltk_widget<Fl_Secret_Input>("Fl_Secret_Input", "Text & Editors", "FL/Fl_Secret_Input.H",
    nullptr, [](Fl_Secret_Input* in) { in->value("SecretPassword123!"); }, "set_secret");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Secret_Input);

static BenchmarkResult bench_Fl_File_Input() {
  return benchmark_fltk_widget<Fl_File_Input>("Fl_File_Input", "Text & Editors", "FL/Fl_File_Input.H",
    nullptr, [](Fl_File_Input* in) { in->value("/usr/local/include/FL/Fl.H"); }, "set_path");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_File_Input);

static BenchmarkResult bench_Fl_Output() {
  return benchmark_fltk_widget<Fl_Output>("Fl_Output", "Text & Editors", "FL/Fl_Output.H",
    nullptr, [](Fl_Output* out) { out->value("Output Text Sample"); }, "set_output");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Output);

static BenchmarkResult bench_Fl_Multiline_Output() {
  return benchmark_fltk_widget<Fl_Multiline_Output>("Fl_Multiline_Output", "Text & Editors", "FL/Fl_Multiline_Output.H",
    nullptr, [](Fl_Multiline_Output* out) { out->value("Multi\nLine\nOutput\n"); }, "set_multi_out");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Multiline_Output);

static BenchmarkResult bench_Fl_Text_Buffer() {
  BenchmarkResult res = benchmark_fltk_class<Fl_Text_Buffer>("Fl_Text_Buffer", "Text & Editors", "FL/Fl_Text_Buffer.H",
    []() { return new Fl_Text_Buffer(); },
    [](Fl_Text_Buffer* buf) {
      buf->append("The quick brown fox jumps over the lazy dog\n");
      if (buf->length() > 5000) buf->text("");
    }, "append_text");

  // Deep memory test: 10,000 lines
  Fl_Text_Buffer deep_buf;
  MemoryStats m0 = get_current_memory();
  for (int i = 0; i < 1000; ++i) {
    deep_buf.append("Line in FLTK text buffer for memory evaluation\n");
  }
  MemoryStats m1 = get_current_memory();
  res.deep_memory_kb = (m1.heap_used_bytes > m0.heap_used_bytes)
    ? (m1.heap_used_bytes - m0.heap_used_bytes) / 1024.0
    : deep_buf.length() / 1024.0;

  return res;
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Text_Buffer);

static BenchmarkResult bench_Fl_Text_Display() {
  return benchmark_fltk_widget<Fl_Text_Display>("Fl_Text_Display", "Text & Editors", "FL/Fl_Text_Display.H",
    [](Fl_Text_Display* td) {
      Fl_Text_Buffer* buf = new Fl_Text_Buffer();
      buf->text("Initial display content");
      td->buffer(buf);
    },
    [](Fl_Text_Display* td) {
      td->insert("More text ");
    }, "insert_display");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Text_Display);

static BenchmarkResult bench_Fl_Text_Editor() {
  return benchmark_fltk_widget<Fl_Text_Editor>("Fl_Text_Editor", "Text & Editors", "FL/Fl_Text_Editor.H",
    [](Fl_Text_Editor* te) {
      Fl_Text_Buffer* buf = new Fl_Text_Buffer();
      buf->text("Initial editor content\n");
      te->buffer(buf);
    },
    [](Fl_Text_Editor* te) {
      te->insert("Edited content ");
    }, "insert_editor");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Text_Editor);

static BenchmarkResult bench_Fl_Terminal() {
  return benchmark_fltk_class<Fl_Terminal>("Fl_Terminal", "Text & Editors", "FL/Fl_Terminal.H",
    []() {
      return new Fl_Terminal(0, 0, 100, 100, "Term", 24, 80, 100);
    },
    [](Fl_Terminal* term) {
      term->append("FLTK Terminal line benchmark\r\n");
    }, "terminal_append");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Terminal);
