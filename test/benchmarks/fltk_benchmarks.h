//
// FLTK Benchmark Suite - Core Framework Header
//
// Fast Light Tool Kit (FLTK)
//

#ifndef FLTK_BENCHMARKS_H
#define FLTK_BENCHMARKS_H

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Image_Surface.H>

#include <chrono>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <functional>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <memory>

#if defined(__linux__) || defined(__unix__)
#include <unistd.h>
#include <sys/resource.h>
#if defined(__GLIBC__)
#include <malloc.h>
#endif
#elif defined(_WIN32)
#include <windows.h>
#include <psapi.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#endif

namespace fltk_bench {

struct MemoryStats {
  size_t resident_bytes; // RSS in bytes
  size_t virtual_bytes;  // VMS in bytes
  size_t heap_used_bytes;// Allocated heap via mallinfo if available
};

inline MemoryStats get_current_memory() {
  MemoryStats stats = {0, 0, 0};

#if defined(__linux__)
  // Read /proc/self/statm
  std::ifstream statm("/proc/self/statm");
  if (statm.is_open()) {
    size_t vms_pages = 0, rss_pages = 0;
    statm >> vms_pages >> rss_pages;
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size > 0) {
      stats.virtual_bytes = vms_pages * static_cast<size_t>(page_size);
      stats.resident_bytes = rss_pages * static_cast<size_t>(page_size);
    }
  }

#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 33)
  struct mallinfo2 mi2 = mallinfo2();
  stats.heap_used_bytes = static_cast<size_t>(mi2.uordblks);
#else
  struct mallinfo mi = mallinfo();
  stats.heap_used_bytes = static_cast<size_t>(mi.uordblks);
#endif
#endif

#elif defined(__APPLE__)
  struct mach_task_basic_info info;
  mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
  if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS) {
    stats.resident_bytes = info.resident_size;
    stats.virtual_bytes = info.virtual_size;
  }
#elif defined(_WIN32)
  PROCESS_MEMORY_COUNTERS_EX pmc;
  if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
    stats.resident_bytes = pmc.WorkingSetSize;
    stats.virtual_bytes = pmc.PrivateUsage;
  }
#endif

  return stats;
}

struct BenchmarkResult {
  std::string class_name;
  std::string category;
  std::string header_file;
  
  // Memory metrics
  size_t sizeof_bytes = 0;
  double heap_bytes_per_instance = 0.0;
  double batch_total_ram_kb = 0.0;
  double deep_memory_kb = 0.0;
  
  // Speed metrics (in nanoseconds)
  double single_create_ns = 0.0;
  double single_destroy_ns = 0.0;
  double batch_create_mops = 0.0; // Million operations per second
  double batch_destroy_mops = 0.0;
  
  // Specific operations
  double layout_or_resize_ns = 0.0;
  double draw_or_calc_ns = 0.0;
  double event_handle_ns = 0.0;
  
  // Extra custom operation (e.g. search, add_item, encode/decode)
  std::string custom_op_name;
  double custom_op_ns = 0.0;
  double custom_op_ops_per_sec = 0.0;
  
  bool passed = true;
  std::string notes;
};

// Benchmark function pointer type
typedef BenchmarkResult (*BenchmarkFunc)();

class BenchmarkRegistry {
public:
  static BenchmarkRegistry& instance() {
    static BenchmarkRegistry registry;
    return registry;
  }

  void add(BenchmarkFunc func) {
    benchmarks_.push_back(func);
  }

  const std::vector<BenchmarkFunc>& benchmarks() const {
    return benchmarks_;
  }

private:
  BenchmarkRegistry() = default;
  std::vector<BenchmarkFunc> benchmarks_;
};

struct BenchmarkRegistrar {
  BenchmarkRegistrar(BenchmarkFunc func) {
    BenchmarkRegistry::instance().add(func);
  }
};

#define FLTK_REGISTER_BENCHMARK(func) \
  static ::fltk_bench::BenchmarkRegistrar _reg_##func(func)

// High-resolution clock helper
using Clock = std::chrono::steady_clock;
using DurationNs = std::chrono::duration<double, std::nano>;

template<typename Func>
inline double measure_ns(Func&& f, int iterations = 1000) {
  if (iterations <= 0) iterations = 1;
  // Warmup
  for (int i = 0; i < std::min(10, iterations); ++i) {
    f();
  }
  auto start = Clock::now();
  for (int i = 0; i < iterations; ++i) {
    f();
  }
  auto end = Clock::now();
  DurationNs elapsed = end - start;
  return elapsed.count() / iterations;
}

// Generic benchmark template for FLTK widgets with constructor (x, y, w, h, label)
template<typename WidgetType>
inline BenchmarkResult benchmark_fltk_widget(const char* class_name,
                                             const char* category,
                                             const char* header,
                                             std::function<void(WidgetType*)> custom_init = nullptr,
                                             std::function<void(WidgetType*)> custom_op = nullptr,
                                             const char* custom_op_name = "")
{
  BenchmarkResult res;
  res.class_name = class_name;
  res.category = category;
  res.header_file = header;
  res.sizeof_bytes = sizeof(WidgetType);

  const int BATCH_SIZE = 500;
  const int TIME_ITERS = 2000;

  Fl_Group::current(nullptr);

  // 1. Measure single create & destroy time
  {
    std::vector<WidgetType*> temp_ptrs;
    temp_ptrs.reserve(100);

    // Warmup
    for (int i = 0; i < 10; ++i) {
      Fl_Group::current(nullptr);
      WidgetType* w = new WidgetType(0, 0, 100, 30, "Test");
      Fl_Group::current(nullptr);
      delete w;
    }

    auto t0 = Clock::now();
    for (int i = 0; i < TIME_ITERS; ++i) {
      Fl_Group::current(nullptr);
      WidgetType* w = new WidgetType(0, 0, 100, 30, "Test");
      Fl_Group::current(nullptr);
      temp_ptrs.push_back(w);
      if (temp_ptrs.size() >= 50) {
        for (auto p : temp_ptrs) delete p;
        temp_ptrs.clear();
      }
    }
    for (auto p : temp_ptrs) delete p;
    temp_ptrs.clear();
    auto t1 = Clock::now();

    DurationNs create_time = t1 - t0;
    res.single_create_ns = create_time.count() / TIME_ITERS;
    if (res.single_create_ns > 0) {
      res.batch_create_mops = 1000.0 / res.single_create_ns;
    }
  }

  // 2. Measure batch destruction
  {
    Fl_Group::current(nullptr);
    std::vector<WidgetType*> batch;
    batch.reserve(BATCH_SIZE);
    for (int i = 0; i < BATCH_SIZE; ++i) {
      Fl_Group::current(nullptr);
      WidgetType* w = new WidgetType(0, 0, 100, 30, "Bench");
      Fl_Group::current(nullptr);
      batch.push_back(w);
    }
    auto t0 = Clock::now();
    for (int i = 0; i < BATCH_SIZE; ++i) {
      delete batch[i];
    }
    auto t1 = Clock::now();
    DurationNs destroy_time = t1 - t0;
    res.single_destroy_ns = destroy_time.count() / BATCH_SIZE;
    if (res.single_destroy_ns > 0) {
      res.batch_destroy_mops = 1000.0 / res.single_destroy_ns;
    }
  }

  // 3. Measure RAM consumption via batch allocation
  {
    Fl_Group::current(nullptr);
    MemoryStats m0 = get_current_memory();
    std::vector<WidgetType*> batch;
    batch.reserve(BATCH_SIZE);
    for (int i = 0; i < BATCH_SIZE; ++i) {
      Fl_Group::current(nullptr);
      WidgetType* w = new WidgetType(0, 0, 100, 30, "RamTest");
      Fl_Group::current(nullptr);
      if (custom_init) custom_init(w);
      Fl_Group::current(nullptr);
      batch.push_back(w);
    }
    MemoryStats m1 = get_current_memory();

    double delta_rss = (m1.resident_bytes > m0.resident_bytes)
      ? static_cast<double>(m1.resident_bytes - m0.resident_bytes)
      : 0.0;
    double delta_heap = (m1.heap_used_bytes > m0.heap_used_bytes)
      ? static_cast<double>(m1.heap_used_bytes - m0.heap_used_bytes)
      : 0.0;

    double effective_delta = (delta_heap > 0) ? delta_heap : delta_rss;
    if (effective_delta <= 0) {
      // Fallback estimate based on sizeof and malloc alignment
      effective_delta = BATCH_SIZE * ((sizeof(WidgetType) + 15) & ~15);
    }

    res.heap_bytes_per_instance = effective_delta / BATCH_SIZE;
    res.batch_total_ram_kb = effective_delta / 1024.0;

    for (int i = 0; i < BATCH_SIZE; ++i) {
      delete batch[i];
    }
  }

  // 4. Measure layout / resize / event handle
  {
    Fl_Group::current(nullptr);
    std::unique_ptr<Fl_Window> dummy_win(new Fl_Window(400, 400));
    dummy_win->begin();
    WidgetType* w = new WidgetType(10, 10, 120, 40, "Ops");
    Fl_Group::current(nullptr);
    dummy_win->end();
    if (custom_init) custom_init(w);
    Fl_Group::current(nullptr);

    // Resize measurement
    res.layout_or_resize_ns = measure_ns([&]() {
      w->resize(10, 10, 140, 50);
      w->resize(10, 10, 120, 40);
    }, 1000) / 2.0;

    // Event handle measurement (FL_MOVE synthetic event)
    res.event_handle_ns = measure_ns([&]() {
      Fl_Widget* fw = static_cast<Fl_Widget*>(w);
      fw->handle(FL_MOVE);
    }, 2000);

    // Custom op if provided
    if (custom_op && custom_op_name && strlen(custom_op_name) > 0) {
      res.custom_op_name = custom_op_name;
      res.custom_op_ns = measure_ns([&]() {
        custom_op(w);
      }, 2000);
      if (res.custom_op_ns > 0) {
        res.custom_op_ops_per_sec = 1e9 / res.custom_op_ns;
      }
    }

    dummy_win->clear();
    Fl_Group::current(nullptr);
  }

  return res;
}

// Generic benchmark template for non-widget FLTK classes (protocols, system, images, tools)
template<typename ClassType, typename FactoryFunc>
inline BenchmarkResult benchmark_fltk_class(const char* class_name,
                                            const char* category,
                                            const char* header,
                                            FactoryFunc factory,
                                            std::function<void(ClassType*)> custom_op = nullptr,
                                            const char* custom_op_name = "")
{
  BenchmarkResult res;
  res.class_name = class_name;
  res.category = category;
  res.header_file = header;
  res.sizeof_bytes = sizeof(ClassType);

  const int BATCH_SIZE = 500;
  const int TIME_ITERS = 2000;

  // 1. Create time
  {
    std::vector<ClassType*> temp_ptrs;
    temp_ptrs.reserve(50);

    auto t0 = Clock::now();
    for (int i = 0; i < TIME_ITERS; ++i) {
      Fl_Group::current(nullptr);
      ClassType* obj = factory();
      Fl_Group::current(nullptr);
      temp_ptrs.push_back(obj);
      if (temp_ptrs.size() >= 50) {
        for (auto p : temp_ptrs) delete p;
        temp_ptrs.clear();
      }
    }
    for (auto p : temp_ptrs) delete p;
    temp_ptrs.clear();
    auto t1 = Clock::now();

    DurationNs create_time = t1 - t0;
    res.single_create_ns = create_time.count() / TIME_ITERS;
    if (res.single_create_ns > 0) {
      res.batch_create_mops = 1000.0 / res.single_create_ns;
    }
  }

  // 2. Destroy time
  {
    Fl_Group::current(nullptr);
    std::vector<ClassType*> batch;
    batch.reserve(BATCH_SIZE);
    for (int i = 0; i < BATCH_SIZE; ++i) {
      Fl_Group::current(nullptr);
      batch.push_back(factory());
      Fl_Group::current(nullptr);
    }
    auto t0 = Clock::now();
    for (int i = 0; i < BATCH_SIZE; ++i) {
      delete batch[i];
    }
    auto t1 = Clock::now();
    DurationNs destroy_time = t1 - t0;
    res.single_destroy_ns = destroy_time.count() / BATCH_SIZE;
    if (res.single_destroy_ns > 0) {
      res.batch_destroy_mops = 1000.0 / res.single_destroy_ns;
    }
  }

  // 3. RAM measurement
  {
    Fl_Group::current(nullptr);
    MemoryStats m0 = get_current_memory();
    std::vector<ClassType*> batch;
    batch.reserve(BATCH_SIZE);
    for (int i = 0; i < BATCH_SIZE; ++i) {
      Fl_Group::current(nullptr);
      batch.push_back(factory());
      Fl_Group::current(nullptr);
    }
    MemoryStats m1 = get_current_memory();

    double delta_rss = (m1.resident_bytes > m0.resident_bytes)
      ? static_cast<double>(m1.resident_bytes - m0.resident_bytes)
      : 0.0;
    double delta_heap = (m1.heap_used_bytes > m0.heap_used_bytes)
      ? static_cast<double>(m1.heap_used_bytes - m0.heap_used_bytes)
      : 0.0;

    double effective_delta = (delta_heap > 0) ? delta_heap : delta_rss;
    if (effective_delta <= 0) {
      effective_delta = BATCH_SIZE * ((sizeof(ClassType) + 15) & ~15);
    }

    res.heap_bytes_per_instance = effective_delta / BATCH_SIZE;
    res.batch_total_ram_kb = effective_delta / 1024.0;

    for (int i = 0; i < BATCH_SIZE; ++i) {
      delete batch[i];
    }
  }

  // 4. Custom op
  if (custom_op && custom_op_name && strlen(custom_op_name) > 0) {
    Fl_Group::current(nullptr);
    ClassType* obj = factory();
    Fl_Group::current(nullptr);
    res.custom_op_name = custom_op_name;
    res.custom_op_ns = measure_ns([&]() {
      custom_op(obj);
    }, 1000);
    if (res.custom_op_ns > 0) {
      res.custom_op_ops_per_sec = 1e9 / res.custom_op_ns;
    }
    delete obj;
  }

  return res;
}

} // namespace fltk_bench

#endif // FLTK_BENCHMARKS_H
