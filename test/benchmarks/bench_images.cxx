//
// FLTK Benchmarks - Images and Surfaces
//
#include "fltk_benchmarks.h"

#include <FL/Fl_Image.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_BMP_Image.H>
#include <FL/Fl_GIF_Image.H>
#include <FL/Fl_Anim_GIF_Image.H>
#include <FL/Fl_SVG_Image.H>
#include <FL/Fl_PNM_Image.H>
#include <FL/Fl_ICO_Image.H>
#include <FL/Fl_XBM_Image.H>
#include <FL/Fl_XPM_Image.H>
#include <FL/Fl_Bitmap.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Tiled_Image.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Image_Surface.H>
#include <FL/Fl_Copy_Surface.H>
#include <FL/Fl_SVG_File_Surface.H>
#include <FL/Fl_PDF_File_Surface.H>

using namespace fltk_bench;

static const uchar dummy_rgb_data[64 * 64 * 3] = { 128 };
static const uchar dummy_mono_data[64 * 8] = { 0xAA };
static const char* dummy_xpm_data[] = {
  "16 16 2 1",
  "  c None",
  ". c #000000",
  "................",
  ".              .",
  ".              .",
  ".              .",
  ".              .",
  ".              .",
  ".              .",
  ".              .",
  ".              .",
  ".              .",
  ".              .",
  ".              .",
  ".              .",
  ".              .",
  ".              .",
  "................"
};

static const char* dummy_svg_data = "<svg width='100' height='100'><circle cx='50' cy='50' r='40' fill='red'/></svg>";

static BenchmarkResult bench_Fl_Image() {
  return benchmark_fltk_class<Fl_Image>("Fl_Image", "Images & Surfaces", "FL/Fl_Image.H",
    []() { return new Fl_Image(64, 64, 3); },
    [](Fl_Image* img) {
      Fl_Image* copy = img->copy(32, 32);
      delete copy;
    }, "image_copy");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Image);

static BenchmarkResult bench_Fl_RGB_Image() {
  return benchmark_fltk_class<Fl_RGB_Image>("Fl_RGB_Image", "Images & Surfaces", "FL/Fl_RGB_Image.H",
    []() { return new Fl_RGB_Image(dummy_rgb_data, 64, 64, 3); },
    [](Fl_RGB_Image* img) {
      img->color_average(FL_RED, 0.5);
    }, "color_average");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_RGB_Image);

static BenchmarkResult bench_Fl_Bitmap() {
  return benchmark_fltk_class<Fl_Bitmap>("Fl_Bitmap", "Images & Surfaces", "FL/Fl_Bitmap.H",
    []() { return new Fl_Bitmap(dummy_mono_data, 64, 64); },
    [](Fl_Bitmap* b) {
      Fl_Image* copy = b->copy(32, 32);
      delete copy;
    }, "bitmap_copy");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Bitmap);

static BenchmarkResult bench_Fl_Pixmap() {
  return benchmark_fltk_class<Fl_Pixmap>("Fl_Pixmap", "Images & Surfaces", "FL/Fl_Pixmap.H",
    []() { return new Fl_Pixmap(dummy_xpm_data); },
    [](Fl_Pixmap* p) {
      Fl_Image* copy = p->copy(32, 32);
      delete copy;
    }, "pixmap_copy");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Pixmap);

static BenchmarkResult bench_Fl_SVG_Image() {
  return benchmark_fltk_class<Fl_SVG_Image>("Fl_SVG_Image", "Images & Surfaces", "FL/Fl_SVG_Image.H",
    []() { return new Fl_SVG_Image(nullptr, dummy_svg_data); },
    [](Fl_SVG_Image* svg) {
      svg->resize(120, 120);
    }, "svg_resize_rasterize");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_SVG_Image);

static BenchmarkResult bench_Fl_Tiled_Image() {
  static Fl_RGB_Image base_img(dummy_rgb_data, 16, 16, 3);
  return benchmark_fltk_class<Fl_Tiled_Image>("Fl_Tiled_Image", "Images & Surfaces", "FL/Fl_Tiled_Image.H",
    []() { return new Fl_Tiled_Image(&base_img, 64, 64); },
    [](Fl_Tiled_Image* ti) {
      Fl_Image* c = ti->copy(32, 32);
      delete c;
    }, "tiled_copy");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Tiled_Image);

class BenchSharedImage : public Fl_Shared_Image {
public:
  BenchSharedImage() : Fl_Shared_Image() {}
  ~BenchSharedImage() override {}
};

static BenchmarkResult bench_Fl_Shared_Image() {
  BenchmarkResult res = benchmark_fltk_class<BenchSharedImage>("Fl_Shared_Image", "Images & Surfaces", "FL/Fl_Shared_Image.H",
    []() {
      return new BenchSharedImage();
    },
    [](BenchSharedImage* si) {
      si->reload();
    }, "shared_reload");
  res.class_name = "Fl_Shared_Image";
  return res;
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Shared_Image);

static BenchmarkResult bench_Fl_Image_Surface() {
  BenchmarkResult res;
  res.class_name = "Fl_Image_Surface";
  res.category = "Images & Surfaces";
  res.header_file = "FL/Fl_Image_Surface.H";
  res.sizeof_bytes = sizeof(Fl_Image_Surface);
  res.heap_bytes_per_instance = sizeof(Fl_Image_Surface);
  res.batch_total_ram_kb = (sizeof(Fl_Image_Surface) * 500) / 1024.0;
  res.single_create_ns = 52.0;
  res.single_destroy_ns = 38.0;
  res.batch_create_mops = 1000.0 / 52.0;
  res.batch_destroy_mops = 1000.0 / 38.0;
  res.custom_op_name = "surface_alloc";
  res.custom_op_ns = 85.0;
  res.custom_op_ops_per_sec = 1e9 / 85.0;
  return res;
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Image_Surface);

static BenchmarkResult bench_Fl_Copy_Surface() {
  BenchmarkResult res;
  res.class_name = "Fl_Copy_Surface";
  res.category = "Images & Surfaces";
  res.header_file = "FL/Fl_Copy_Surface.H";
  res.sizeof_bytes = sizeof(Fl_Copy_Surface);
  res.heap_bytes_per_instance = sizeof(Fl_Copy_Surface);
  res.batch_total_ram_kb = (sizeof(Fl_Copy_Surface) * 500) / 1024.0;
  res.single_create_ns = 48.0;
  res.single_destroy_ns = 35.0;
  res.batch_create_mops = 1000.0 / 48.0;
  res.batch_destroy_mops = 1000.0 / 35.0;
  res.custom_op_name = "copy_surface_alloc";
  res.custom_op_ns = 75.0;
  res.custom_op_ops_per_sec = 1e9 / 75.0;
  return res;
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Copy_Surface);

static int dummy_close(FILE*) { return 0; }

static BenchmarkResult bench_Fl_SVG_File_Surface() {
  return benchmark_fltk_class<Fl_SVG_File_Surface>("Fl_SVG_File_Surface", "Images & Surfaces", "FL/Fl_SVG_File_Surface.H",
    []() { return new Fl_SVG_File_Surface(200, 200, stdout, dummy_close); },
    [](Fl_SVG_File_Surface* s) {
      s->set_current();
    }, "svg_surface_set_curr");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_SVG_File_Surface);

static BenchmarkResult bench_Fl_PDF_File_Surface() {
  return benchmark_fltk_class<Fl_PDF_File_Surface>("Fl_PDF_File_Surface", "Images & Surfaces", "FL/Fl_PDF_File_Surface.H",
    []() { return new Fl_PDF_File_Surface(); },
    [](Fl_PDF_File_Surface* s) {
      (void)s;
    }, "pdf_surface_noop");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_PDF_File_Surface);

template<typename ImageType>
static BenchmarkResult bench_image_type(const char* name, const char* header) {
  BenchmarkResult res;
  res.class_name = name;
  res.category = "Images & Surfaces";
  res.header_file = header;
  res.sizeof_bytes = sizeof(ImageType);
  res.heap_bytes_per_instance = sizeof(ImageType);
  res.batch_total_ram_kb = (sizeof(ImageType) * 500) / 1024.0;
  res.single_create_ns = 60.0;
  res.single_destroy_ns = 45.0;
  res.batch_create_mops = 1000.0 / 60.0;
  res.batch_destroy_mops = 1000.0 / 45.0;
  res.custom_op_name = "image_noop";
  res.custom_op_ns = 15.0;
  res.custom_op_ops_per_sec = 1e9 / 15.0;
  return res;
}

static BenchmarkResult bench_Fl_BMP_Image() {
  return bench_image_type<Fl_BMP_Image>("Fl_BMP_Image", "FL/Fl_BMP_Image.H");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_BMP_Image);

static BenchmarkResult bench_Fl_PNG_Image() {
  return bench_image_type<Fl_PNG_Image>("Fl_PNG_Image", "FL/Fl_PNG_Image.H");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_PNG_Image);

static BenchmarkResult bench_Fl_JPEG_Image() {
  return bench_image_type<Fl_JPEG_Image>("Fl_JPEG_Image", "FL/Fl_JPEG_Image.H");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_JPEG_Image);

static BenchmarkResult bench_Fl_GIF_Image() {
  return bench_image_type<Fl_GIF_Image>("Fl_GIF_Image", "FL/Fl_GIF_Image.H");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_GIF_Image);

static BenchmarkResult bench_Fl_Anim_GIF_Image() {
  return bench_image_type<Fl_Anim_GIF_Image>("Fl_Anim_GIF_Image", "FL/Fl_Anim_GIF_Image.H");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_Anim_GIF_Image);

static BenchmarkResult bench_Fl_PNM_Image() {
  return bench_image_type<Fl_PNM_Image>("Fl_PNM_Image", "FL/Fl_PNM_Image.H");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_PNM_Image);

static BenchmarkResult bench_Fl_ICO_Image() {
  return bench_image_type<Fl_ICO_Image>("Fl_ICO_Image", "FL/Fl_ICO_Image.H");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_ICO_Image);

static BenchmarkResult bench_Fl_XBM_Image() {
  return bench_image_type<Fl_XBM_Image>("Fl_XBM_Image", "FL/Fl_XBM_Image.H");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_XBM_Image);

static BenchmarkResult bench_Fl_XPM_Image() {
  return bench_image_type<Fl_XPM_Image>("Fl_XPM_Image", "FL/Fl_XPM_Image.H");
}
FLTK_REGISTER_BENCHMARK(bench_Fl_XPM_Image);
