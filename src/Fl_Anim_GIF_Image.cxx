//
// Fl_Anim_GIF_Image class for the Fast Light Tool Kit (FLTK).
//
// Copyright 2016-2023 by Christian Grabner <wcout@gmx.net>.
// Copyright 2024-2026 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     https://www.fltk.org/COPYING.php
//
// Please see the following page on how to report bugs and issues:
//
//     https://www.fltk.org/bugs.php
//

#include <FL/Fl.H>
#include <FL/Fl_GIF_Image.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Graphics_Driver.H>
#include <FL/Fl_Group.H>
#include <FL/fl_string_functions.h>
#include <FL/Fl_Anim_GIF_Image.H>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>

bool Fl_GIF_Image::animate = false;

///////////////////////////////////////////////////////////////////////
//  Internal helper classes/structs
///////////////////////////////////////////////////////////////////////

class Fl_Anim_GIF_Image::FrameInfo {
  friend class Fl_Anim_GIF_Image;

public:
  enum class Transparency : uint8_t {
    T_FULL = 0U,
    T_NONE = 0xFFU
  };

  enum class Dispose : uint8_t {
    DISPOSE_UNDEF = 0U,
    DISPOSE_BACKGROUND = 2U,
    DISPOSE_PREVIOUS = 3U
  };

  class RGBA_Color {
  private:
    std::uint8_t m_r{0U};
    std::uint8_t m_g{0U};
    std::uint8_t m_b{0U};
    std::uint8_t m_alpha{static_cast<std::uint8_t>(Transparency::T_NONE)};

  public:
    constexpr RGBA_Color() = default;
    constexpr RGBA_Color(std::uint8_t r_val, std::uint8_t g_val, std::uint8_t b_val, std::uint8_t a_val = static_cast<std::uint8_t>(Transparency::T_NONE))
      : m_r(r_val), m_g(g_val), m_b(b_val), m_alpha(a_val) {}

    std::uint8_t r() const { return m_r; }
    std::uint8_t g() const { return m_g; }
    std::uint8_t b() const { return m_b; }
    std::uint8_t alpha() const { return m_alpha; }
    void set_alpha(std::uint8_t a_val) { m_alpha = a_val; }
  };

  class GifFrame {
  private:
    Fl_RGB_Image *m_rgb{nullptr};
    Fl_Shared_Image *m_scalable{nullptr};
    double m_delay{0.0};
    Fl_Color m_average_color{FL_BLACK};
    float m_average_weight{-1.0F};
    Dispose m_dispose{Dispose::DISPOSE_UNDEF};
    int m_transparent_color_index{-1};
    std::uint16_t m_x{0U};
    std::uint16_t m_y{0U};
    std::uint16_t m_w{0U};
    std::uint16_t m_h{0U};
    RGBA_Color m_transparent_color{};
    bool m_desaturated{false};

  public:
    GifFrame() = default;

    Fl_RGB_Image *rgb() const { return m_rgb; }
    void set_rgb(Fl_RGB_Image *val) { m_rgb = val; }

    Fl_Shared_Image *scalable() const { return m_scalable; }
    void set_scalable(Fl_Shared_Image *val) { m_scalable = val; }

    double delay() const { return m_delay; }
    void set_delay(const double val) { m_delay = val; }

    Fl_Color average_color() const { return m_average_color; }
    void set_average_color(const Fl_Color val) { m_average_color = val; }

    float average_weight() const { return m_average_weight; }
    void set_average_weight(const float val) { m_average_weight = val; }

    Dispose dispose() const { return m_dispose; }
    void set_dispose(const Dispose val) { m_dispose = val; }

    int transparent_color_index() const { return m_transparent_color_index; }
    void set_transparent_color_index(const int val) { m_transparent_color_index = val; }

    std::uint16_t x() const { return m_x; }
    void set_x(const std::uint16_t val) { m_x = val; }

    std::uint16_t y() const { return m_y; }
    void set_y(const std::uint16_t val) { m_y = val; }

    std::uint16_t w() const { return m_w; }
    void set_w(const std::uint16_t val) { m_w = val; }

    std::uint16_t h() const { return m_h; }
    void set_h(const std::uint16_t val) { m_h = val; }

    const RGBA_Color &transparent_color() const { return m_transparent_color; }
    void set_transparent_color(const RGBA_Color &val) { m_transparent_color = val; }

    bool desaturated() const { return m_desaturated; }
    void set_desaturated(const bool val) { m_desaturated = val; }
  };

  explicit FrameInfo(Fl_Anim_GIF_Image *anim_ptr);
  ~FrameInfo();

  FrameInfo(const FrameInfo &) = delete;
  FrameInfo &operator=(const FrameInfo &) = delete;
  FrameInfo(FrameInfo &&) = delete;
  FrameInfo &operator=(FrameInfo &&) = delete;

  void clear();
  void copy(const FrameInfo &fi);
  double convert_delay(int d) const;
  int debug() const { return debug_; }
  bool load(const char *name, const unsigned char *data, size_t length);
  bool push_back_frame(const GifFrame &frame_elem);
  void resize(int W, int H);
  void scale_frame(int frame_idx);
  void set_frame(int frame_idx);

private:
  Fl_Anim_GIF_Image *anim{nullptr};
  std::vector<GifFrame> frames{};
  std::unique_ptr<std::uint8_t[]> offscreen{nullptr};
  GifFrame frame{};
  float average_weight{-1.0F};
  Fl_Color average_color{FL_BLACK};
  Fl_RGB_Scaling scaling{static_cast<Fl_RGB_Scaling>(0)};
  bool valid{false};
  bool desaturate{false};
  bool optimize_mem{false};
  int loop_count{1};
  int loop{0};
  int background_color_index{-1};
  int canvas_w{0};
  int canvas_h{0};
  int debug_{0};
  RGBA_Color background_color{};

  void dispose(int frame_idx);
  void on_frame_data(Fl_GIF_Image::GIF_FRAME &gf);
  void on_extension_data(const Fl_GIF_Image::GIF_FRAME &gf);
  void set_to_background(int frame_idx) const;
};

Fl_Anim_GIF_Image::FrameInfo::FrameInfo(Fl_Anim_GIF_Image *anim_ptr)
  : anim(anim_ptr) {}

Fl_Anim_GIF_Image::FrameInfo::~FrameInfo() {
  clear();
}

void Fl_Anim_GIF_Image::FrameInfo::clear() {
  for (auto &f : frames) {
    if (f.scalable() != nullptr) {
      f.scalable()->release();
      f.set_scalable(nullptr);
    }
    delete f.rgb();
    f.set_rgb(nullptr);
  }
  frames.clear();
  offscreen.reset();
}

double Fl_Anim_GIF_Image::FrameInfo::convert_delay(int d) const {
  int delay_val = d;
  if (delay_val <= 0) {
    delay_val = (loop_count != 1) ? 10 : 0;
  }
  return static_cast<double>(delay_val) / 100.0;
}

void Fl_Anim_GIF_Image::FrameInfo::copy(const FrameInfo &fi) {
  const auto fi_size = static_cast<int>(fi.frames.size());
  for (int i = 0; i < fi_size; ++i) {
    if (!push_back_frame(fi.frames[static_cast<size_t>(i)])) {
      break;
    }
    const double scale_factor_x = static_cast<double>(canvas_w) / static_cast<double>(fi.canvas_w);
    const double scale_factor_y = static_cast<double>(canvas_h) / static_cast<double>(fi.canvas_h);
    const auto u_idx = static_cast<size_t>(i);

    if (fi.optimize_mem) {
      frames[u_idx].set_x(static_cast<std::uint16_t>(std::round(static_cast<double>(fi.frames[u_idx].x()) * scale_factor_x)));
      frames[u_idx].set_y(static_cast<std::uint16_t>(std::round(static_cast<double>(fi.frames[u_idx].y()) * scale_factor_y)));
      frames[u_idx].set_w(static_cast<std::uint16_t>(std::round(static_cast<double>(fi.frames[u_idx].w()) * scale_factor_x)));
      frames[u_idx].set_h(static_cast<std::uint16_t>(std::round(static_cast<double>(fi.frames[u_idx].h()) * scale_factor_y)));
    }

    if (fi.frames[u_idx].rgb() != nullptr) {
      frames[u_idx].set_rgb(dynamic_cast<Fl_RGB_Image*>(fi.frames[u_idx].rgb()->copy()));
    } else {
      frames[u_idx].set_rgb(nullptr);
    }
    frames[u_idx].set_scalable(nullptr);
  }
  optimize_mem = fi.optimize_mem;
  scaling = Fl_Image::RGB_scaling();
  loop_count = fi.loop_count;
}

void Fl_Anim_GIF_Image::FrameInfo::dispose(int frame_idx) {
  if (frame_idx >= 0) {
    const auto u_frame = static_cast<size_t>(frame_idx);
    switch (frames[u_frame].dispose()) {
      case Dispose::DISPOSE_PREVIOUS: {
        int prev = frame_idx;
        while ((prev > 0) && (frames[static_cast<size_t>(prev)].dispose() == Dispose::DISPOSE_PREVIOUS)) {
          --prev;
        }
        if ((prev == 0) && (frames[0U].dispose() == Dispose::DISPOSE_PREVIOUS)) {
          set_to_background(frame_idx);
        } else if (offscreen != nullptr) {
          const auto u_prev = static_cast<size_t>(prev);
          std::uint8_t *dst = offscreen.get();
          const int px = static_cast<int>(frames[u_prev].x());
          const int py = static_cast<int>(frames[u_prev].y());
          int pw = static_cast<int>(frames[u_prev].w());
          int ph = static_cast<int>(frames[u_prev].h());

          if (frames[u_prev].rgb() != nullptr) {
            const auto *src = reinterpret_cast<const std::uint8_t*>(frames[u_prev].rgb()->data()[0]);
            if ((px == 0) && (py == 0) && (pw == canvas_w) && (ph == canvas_h)) {
              const size_t total_bytes = static_cast<size_t>(canvas_w) * static_cast<size_t>(canvas_h) * 4U;
              (void)std::copy_n(src, total_bytes, dst);
            } else {
              if ((px + pw) > canvas_w) {
                pw = canvas_w - px;
              }
              if ((py + ph) > canvas_h) {
                ph = canvas_h - py;
              }
              const size_t row_bytes = static_cast<size_t>(pw) * 4U;
              const size_t src_stride = static_cast<size_t>(frames[u_prev].w()) * 4U;
              for (int y = 0; y < ph; ++y) {
                const size_t dst_offset = (static_cast<size_t>(y + py) * static_cast<size_t>(canvas_w) * 4U) + (static_cast<size_t>(px) * 4U);
                const size_t src_offset = static_cast<size_t>(y) * src_stride;
                (void)std::copy_n(src + src_offset, row_bytes, dst + dst_offset);
              }
            }
          }
        } else {
          // No offscreen buffer
        }
        break;
      }
      case Dispose::DISPOSE_BACKGROUND:
        set_to_background(frame_idx);
        break;
      default:
        break;
    }
  }
}

bool Fl_Anim_GIF_Image::FrameInfo::load(const char *name, const unsigned char *data, size_t length) {
  valid = false;
  if (anim != nullptr) {
    anim->ld(0);
    if (data != nullptr) {
      anim->Fl_GIF_Image::load(name, data, length, true);
    } else {
      anim->Fl_GIF_Image::load(name, true);
    }
  }
  offscreen.reset();
  return valid;
}

void Fl_Anim_GIF_Image::FrameInfo::on_extension_data(const Fl_GIF_Image::GIF_FRAME &gf) {
  if (gf.bptr != nullptr) {
    const std::uint8_t *ext = reinterpret_cast<const std::uint8_t*>(gf.bptr);
    if (std::equal(ext, ext + 11U, reinterpret_cast<const std::uint8_t*>("NETSCAPE2.0"))) {
      const std::uint8_t *params = ext + 11U;
      loop_count = static_cast<int>(params[1]) | (static_cast<int>(params[2]) << 8U);
    }
  }
}

void Fl_Anim_GIF_Image::FrameInfo::on_frame_data(Fl_GIF_Image::GIF_FRAME &gf) {
  if (gf.bptr != nullptr) {
    int delay = gf.delay;
    if (delay <= 0) {
      delay = -(delay + 1);
    }

    if (gf.ifrm == 0) {
      valid = true;
      canvas_w = gf.width;
      canvas_h = gf.height;
      const size_t total_bytes = static_cast<size_t>(canvas_w) * static_cast<size_t>(canvas_h) * 4U;
      offscreen = std::unique_ptr<std::uint8_t[]>(new std::uint8_t[total_bytes]);
      (void)std::fill_n(offscreen.get(), total_bytes, 0U);

      background_color_index = ((gf.clrs != 0) && (gf.bkgd < gf.clrs)) ? gf.bkgd : -1;
      if ((background_color_index >= 0) && (gf.cpal != nullptr)) {
        background_color = RGBA_Color(static_cast<std::uint8_t>(gf.cpal[background_color_index].r),
                                      static_cast<std::uint8_t>(gf.cpal[background_color_index].g),
                                      static_cast<std::uint8_t>(gf.cpal[background_color_index].b));
      }
    }

    frame.set_x(static_cast<std::uint16_t>(gf.x));
    frame.set_y(static_cast<std::uint16_t>(gf.y));
    frame.set_w(static_cast<std::uint16_t>(gf.w));
    frame.set_h(static_cast<std::uint16_t>(gf.h));
    frame.set_delay(convert_delay(delay));
    frame.set_transparent_color_index(((gf.trans != 0) && (gf.trans < gf.clrs)) ? gf.trans : -1);
    frame.set_dispose(static_cast<Dispose>(gf.dispose));

    if ((frame.transparent_color_index() >= 0) && (gf.cpal != nullptr)) {
      frame.set_transparent_color(RGBA_Color(static_cast<std::uint8_t>(gf.cpal[frame.transparent_color_index()].r),
                                             static_cast<std::uint8_t>(gf.cpal[frame.transparent_color_index()].g),
                                             static_cast<std::uint8_t>(gf.cpal[frame.transparent_color_index()].b)));
    }

    dispose(static_cast<int>(frames.size()) - 1);

    if (offscreen != nullptr) {
      const std::uint8_t *bits = reinterpret_cast<const std::uint8_t*>(gf.bptr);
      const size_t total_canvas = static_cast<size_t>(canvas_w) * static_cast<size_t>(canvas_h) * 4U;
      std::uint8_t *off_start = offscreen.get();
      const std::uint8_t *endp = off_start + total_canvas;

      for (int y = static_cast<int>(frame.y()); y < (static_cast<int>(frame.y()) + static_cast<int>(frame.h())); ++y) {
        for (int x = static_cast<int>(frame.x()); x < (static_cast<int>(frame.x()) + static_cast<int>(frame.w())); ++x) {
          const std::uint8_t c = *bits++;
          if (c == static_cast<std::uint8_t>(gf.trans)) {
            continue;
          }
          std::uint8_t *buf = off_start + (static_cast<size_t>(y) * static_cast<size_t>(canvas_w) * 4U + (static_cast<size_t>(x) * 4U));
          if (buf >= endp) {
            continue;
          }
          if (gf.cpal != nullptr) {
            *buf++ = static_cast<std::uint8_t>(gf.cpal[c].r);
            *buf++ = static_cast<std::uint8_t>(gf.cpal[c].g);
            *buf++ = static_cast<std::uint8_t>(gf.cpal[c].b);
            *buf = static_cast<std::uint8_t>(Transparency::T_NONE);
          }
        }
      }

      if (optimize_mem) {
        const size_t frame_bytes = static_cast<size_t>(frame.w()) * static_cast<size_t>(frame.h()) * 4U;
        auto *buf = new std::uint8_t[frame_bytes];
        std::uint8_t *dest = buf;
        for (int y = static_cast<int>(frame.y()); y < (static_cast<int>(frame.y()) + static_cast<int>(frame.h())); ++y) {
          for (int x = static_cast<int>(frame.x()); x < (static_cast<int>(frame.x()) + static_cast<int>(frame.w())); ++x) {
            const std::uint8_t *src = off_start + (static_cast<size_t>(y) * static_cast<size_t>(canvas_w) * 4U + static_cast<size_t>(x) * 4U);
            if (src < endp) {
              (void)std::copy_n(src, 4U, dest);
            }
            dest += 4;
          }
        }
        frame.set_rgb(new Fl_RGB_Image(buf, frame.w(), frame.h(), 4));
      } else {
        auto *buf = new std::uint8_t[total_canvas];
        (void)std::copy_n(off_start, total_canvas, buf);
        frame.set_rgb(new Fl_RGB_Image(buf, canvas_w, canvas_h, 4));
      }

      if (frame.rgb() != nullptr) {
        frame.rgb()->alloc_array = static_cast<char>(1);
      }
    }

    if (!push_back_frame(frame)) {
      valid = false;
    }
  }
}

bool Fl_Anim_GIF_Image::FrameInfo::push_back_frame(const GifFrame &frame_elem) {
  frames.push_back(frame_elem);
  return true;
}

void Fl_Anim_GIF_Image::FrameInfo::resize(int W, int H) {
  const double scale_factor_x = static_cast<double>(W) / static_cast<double>(canvas_w);
  const double scale_factor_y = static_cast<double>(H) / static_cast<double>(canvas_h);

  for (auto &f : frames) {
    if (optimize_mem) {
      f.set_x(static_cast<std::uint16_t>(std::round(static_cast<double>(f.x()) * scale_factor_x)));
      f.set_y(static_cast<std::uint16_t>(std::round(static_cast<double>(f.y()) * scale_factor_y)));
      f.set_w(static_cast<std::uint16_t>(std::round(static_cast<double>(f.w()) * scale_factor_x)));
      f.set_h(static_cast<std::uint16_t>(std::round(static_cast<double>(f.h()) * scale_factor_y)));
    }
  }
  canvas_w = W;
  canvas_h = H;
}

void Fl_Anim_GIF_Image::FrameInfo::scale_frame(int frame_idx) {
  if ((frame_idx >= 0) && (static_cast<size_t>(frame_idx) < frames.size())) {
    const auto u_frame = static_cast<size_t>(frame_idx);
    const int new_w = optimize_mem ? static_cast<int>(frames[u_frame].w()) : canvas_w;
    const int new_h = optimize_mem ? static_cast<int>(frames[u_frame].h()) : canvas_h;

    if ((frames[u_frame].scalable() == nullptr) ||
        (frames[u_frame].scalable()->w() != new_w) ||
        (frames[u_frame].scalable()->h() != new_h)) {
      const Fl_RGB_Scaling old_scaling = Fl_Image::RGB_scaling();
      Fl_Image::RGB_scaling(scaling);
      if (frames[u_frame].scalable() == nullptr) {
        frames[u_frame].set_scalable(Fl_Shared_Image::get(frames[u_frame].rgb(), 0));
      }
      if (frames[u_frame].scalable() != nullptr) {
        frames[u_frame].scalable()->scale(new_w, new_h, 0, 1);
      }
      Fl_Image::RGB_scaling(old_scaling);
    }
  }
}

void Fl_Anim_GIF_Image::FrameInfo::set_to_background(const int frame_idx) const {
  if (offscreen != nullptr) {
    int bg = background_color_index;
    const int tp = (frame_idx >= 0) ? frames[static_cast<size_t>(frame_idx)].transparent_color_index() : bg;

    RGBA_Color color = background_color;
    if (frame_idx >= 0) {
      if (tp >= 0) {
        color = frames[static_cast<size_t>(frame_idx)].transparent_color();
      }
    }
    if (tp >= 0 && bg >= 0) {
      bg = tp;
    }

    if (tp == bg || tp < 0) {
      color.set_alpha(static_cast<std::uint8_t>(Transparency::T_FULL));
    } else {
      color.set_alpha(static_cast<std::uint8_t>(Transparency::T_NONE));
    }

    const size_t total_canvas = static_cast<size_t>(canvas_w) * static_cast<size_t>(canvas_h) * 4U;
    std::uint8_t *start = offscreen.get();
    for (std::uint8_t *p = start + total_canvas - 4U; p >= start; p -= 4) {
      const auto *color_bytes = reinterpret_cast<const std::uint8_t*>(&color);
      (void)std::copy_n(color_bytes, 4U, p);
    }
  }
}

void Fl_Anim_GIF_Image::FrameInfo::set_frame(const int frame_idx) {
  if (frame_idx >= 0 && static_cast<size_t>(frame_idx) < frames.size()) {
    const auto u_frame = static_cast<size_t>(frame_idx);
    scale_frame(frame_idx);

    if ((average_weight >= 0.0F) && (average_weight < 1.0F) &&
        ((average_color != frames[u_frame].average_color()) ||
         (average_weight != frames[u_frame].average_weight()))) {
      if (frames[u_frame].rgb() != nullptr) {
        frames[u_frame].rgb()->color_average(average_color, average_weight);
      }
      frames[u_frame].set_average_color(average_color);
      frames[u_frame].set_average_weight(average_weight);
    }

    if (desaturate && !frames[u_frame].desaturated()) {
      if (frames[u_frame].rgb() != nullptr) {
        frames[u_frame].rgb()->desaturate();
      }
      frames[u_frame].set_desaturated(true);
    }
  }
}

///////////////////////////////////////////////////////////////////////
// Fl_Anim_GIF_Image global variables
///////////////////////////////////////////////////////////////////////

double Fl_Anim_GIF_Image::min_delay = 0.0;
bool Fl_Anim_GIF_Image::loop = true;

///////////////////////////////////////////////////////////////////////
// class Fl_Anim_GIF_Image implementation
///////////////////////////////////////////////////////////////////////

Fl_Anim_GIF_Image::Fl_Anim_GIF_Image(const char *filename,
                                     Fl_Widget *canvas_widget,
                                     const unsigned short flags)
  : Fl_GIF_Image(),
    name_(nullptr),
    flags_(flags),
    canvas_(canvas_widget),
    uncache_(false),
    valid_(false),
    frame_(-1),
    speed_(1.0),
    fi_(new FrameInfo(this)) {
  const auto log_val = static_cast<unsigned short>(flags_ & static_cast<unsigned short>(LOG_FLAG));
  const auto debug_val = static_cast<unsigned short>(flags_ & static_cast<unsigned short>(DEBUG_FLAG));
  fi_->debug_ = ((log_val != 0U) ? 1 : 0) + 2 * ((debug_val != 0U) ? 1 : 0);

  const auto opt_val = static_cast<unsigned short>(flags_ & static_cast<unsigned short>(OPTIMIZE_MEMORY));
  fi_->optimize_mem = (opt_val != 0U);
  valid_ = load(filename, nullptr, 0U);

  if ((canvas_w() != 0) && (canvas_h() != 0)) {
    if ((w() == 0) && (h() == 0)) {
      w(canvas_w());
      h(canvas_h());
    }
  }
  canvas(canvas_widget, flags);

  const auto dont_start_val = static_cast<unsigned short>(flags & static_cast<unsigned short>(DONT_START));
  if (dont_start_val == 0U) {
    (void)start();
  } else {
    frame_ = 0;
  }
}

Fl_Anim_GIF_Image::Fl_Anim_GIF_Image(const char* imagename, const unsigned char *data,
                                     const size_t length, Fl_Widget *canvas_widget,
                                     const unsigned short flags)
  : Fl_GIF_Image(),
    name_(nullptr),
    flags_(flags),
    canvas_(canvas_widget),
    uncache_(false),
    valid_(false),
    frame_(-1),
    speed_(1.0),
    fi_(new FrameInfo(this)) {
  const auto log_val = static_cast<unsigned short>(flags_ & static_cast<unsigned short>(LOG_FLAG));
  const auto debug_val = static_cast<unsigned short>(flags_ & static_cast<unsigned short>(DEBUG_FLAG));
  fi_->debug_ = ((log_val != 0U) ? 1 : 0) + 2 * ((debug_val != 0U) ? 1 : 0);

  const auto opt_val = static_cast<unsigned short>(flags_ & static_cast<unsigned short>(OPTIMIZE_MEMORY));
  fi_->optimize_mem = (opt_val != 0U);
  valid_ = load(imagename, data, length);

  if ((canvas_w() != 0) && (canvas_h() != 0)) {
    if ((w() == 0) && (h() == 0)) {
      w(canvas_w());
      h(canvas_h());
    }
  }
  canvas(canvas_widget, flags);

  const auto dont_start_val = static_cast<unsigned short>(flags & static_cast<unsigned short>(DONT_START));
  if (dont_start_val == 0U) {
    (void)start();
  } else {
    frame_ = 0;
  }
}

Fl_Anim_GIF_Image::Fl_Anim_GIF_Image()
  : name_(nullptr),
    flags_(0U),
    canvas_(nullptr),
    uncache_(false),
    valid_(false),
    frame_(-1),
    speed_(1.0),
    fi_(new FrameInfo(this)) {}

Fl_Anim_GIF_Image::~Fl_Anim_GIF_Image() {
  Fl::remove_timeout(cb_animate, this);
  delete fi_;
  std::free(name_);
}

void Fl_Anim_GIF_Image::canvas(Fl_Widget *canvas_widget, const unsigned short flags) {
  if (canvas_ != nullptr) {
    canvas_->image(nullptr);
  }
  canvas_ = canvas_widget;

  const auto dont_set_img = static_cast<unsigned short>(flags & static_cast<unsigned short>(DONT_SET_AS_IMAGE));
  if ((canvas_ != nullptr) && (dont_set_img == 0U)) {
    canvas_->image(this);
  }

  const auto dont_resize = static_cast<unsigned short>(flags & static_cast<unsigned short>(DONT_RESIZE_CANVAS));
  if ((canvas_ != nullptr) && (dont_resize == 0U)) {
    canvas_->size(w(), h());
  }

  if (flags_ != flags) {
    flags_ = flags;
    const auto log_val = static_cast<unsigned short>(flags & static_cast<unsigned short>(LOG_FLAG));
    const auto debug_val = static_cast<unsigned short>(flags & static_cast<unsigned short>(DEBUG_FLAG));
    fi_->debug_ = ((log_val != 0U) ? 1 : 0) + 2 * ((debug_val != 0U) ? 1 : 0);
  }

  frame_ = -1;
  if (Fl::has_timeout(cb_animate, this) != 0) {
    Fl::remove_timeout(cb_animate, this);
    (void)next_frame();
  } else if (!fi_->frames.empty()) {
    set_frame(0);
  } else {
    // No-op
  }
}

Fl_Widget *Fl_Anim_GIF_Image::canvas() const {
  return canvas_;
}

int Fl_Anim_GIF_Image::canvas_w() const {
  return fi_->canvas_w;
}

int Fl_Anim_GIF_Image::canvas_h() const {
  return fi_->canvas_h;
}

void Fl_Anim_GIF_Image::cb_animate(void *d) {
  if (d != nullptr) {
    auto *b = static_cast<Fl_Anim_GIF_Image*>(d);
    (void)b->next_frame();
  }
}

void Fl_Anim_GIF_Image::clear_frames() {
  fi_->clear();
  valid_ = false;
}

void Fl_Anim_GIF_Image::color_average(const Fl_Color c, const float i) {
  float blend = i;
  if (blend < 0.0F) {
    blend = -blend;
    for (int f = 0; f < frames(); ++f) {
      if (fi_->frames[static_cast<size_t>(f)].rgb() != nullptr) {
        fi_->frames[static_cast<size_t>(f)].rgb()->color_average(c, blend);
      }
    }
  } else {
    fi_->average_color = c;
    fi_->average_weight = blend;
  }
}

Fl_Image *Fl_Anim_GIF_Image::copy(int W, int H) const {
  auto *copied = new Fl_Anim_GIF_Image();
  if (!fi_->frames.empty()) {
    auto *gif = dynamic_cast<Fl_Pixmap*>(Fl_GIF_Image::copy(W, H));
    if (gif != nullptr) {
      copied->Fl_GIF_Image::data(gif->data(), gif->count());
      copied->alloc_data = gif->alloc_data;
      gif->alloc_data = 0;
      delete gif;
    }
  }

  if (name_ != nullptr) {
    copied->name_ = fl_strdup(name_);
  }
  copied->flags_ = flags_;
  copied->frame_ = frame_;
  copied->speed_ = speed_;

  copied->w(W);
  copied->h(H);
  copied->fi_->canvas_w = W;
  copied->fi_->canvas_h = H;
  copied->fi_->copy(*fi_);

  copied->uncache_ = uncache_;
  copied->valid_ = valid_ && (copied->fi_->frames.size() == fi_->frames.size());
  copied->scale_frame();

  if (copied->valid_ && (frame_ >= 0) && (Fl::has_timeout(cb_animate, copied) == 0)) {
    (void)copied->start();
  }
  return copied;
}

int Fl_Anim_GIF_Image::debug() const {
  return fi_->debug();
}

double Fl_Anim_GIF_Image::delay(const int frame_idx) const {
  double res = 0.0;
  if ((frame_idx >= 0) && (frame_idx < frames())) {
    res = fi_->frames[static_cast<size_t>(frame_idx)].delay();
  }
  return res;
}

void Fl_Anim_GIF_Image::delay(const int frame_idx, const double delay_val) const {
  if ((frame_idx >= 0) && (frame_idx < frames())) {
    fi_->frames[static_cast<size_t>(frame_idx)].set_delay(delay_val);
  }
}

void Fl_Anim_GIF_Image::desaturate() {
  fi_->desaturate = true;
  set_frame();
}

void Fl_Anim_GIF_Image::draw(const int x, const int y, const int w, const int h, const int cx,
                             const int cy) {
  if (this->image() != nullptr) {
    if (fi_->optimize_mem) {
      int f0 = frame_;
      while ((f0 > 0) && !((fi_->frames[static_cast<size_t>(f0)].x() == 0U) &&
                           (fi_->frames[static_cast<size_t>(f0)].y() == 0U) &&
                           (static_cast<int>(fi_->frames[static_cast<size_t>(f0)].w()) == this->w()) &&
                           (static_cast<int>(fi_->frames[static_cast<size_t>(f0)].h()) == this->h()))) {
        --f0;
      }
      for (int f = f0; f <= frame_; ++f) {
        const auto u_f = static_cast<size_t>(f);
        if ((f < frame_) && (fi_->frames[u_f].dispose() == FrameInfo::Dispose::DISPOSE_PREVIOUS)) {
          continue;
        }
        if ((f < frame_) && (fi_->frames[u_f].dispose() == FrameInfo::Dispose::DISPOSE_BACKGROUND)) {
          continue;
        }
        Fl_RGB_Image *rgb = fi_->frames[u_f].rgb();
        if (rgb != nullptr) {
          const float s = Fl_Graphics_Driver::default_driver().scale();
          rgb->scale(static_cast<int>(s * static_cast<float>(fi_->frames[u_f].w())),
                     static_cast<int>(s * static_cast<float>(fi_->frames[u_f].h())), 0, 1);
          rgb->draw(static_cast<int>(static_cast<float>(x) + (s * static_cast<float>(fi_->frames[u_f].x()))),
                    static_cast<int>(static_cast<float>(y) + (s * static_cast<float>(fi_->frames[u_f].y()))),
                    w, h, cx, cy);
        }
      }
    } else {
      this->image()->scale(Fl_GIF_Image::w(), Fl_GIF_Image::h(), 0, 1);
      this->image()->draw(x, y, w, h, cx, cy);
    }
  } else {
    Fl_GIF_Image::draw(x, y, w, h, cx, cy);
  }
}

int Fl_Anim_GIF_Image::frame() const {
  return frame_;
}

void Fl_Anim_GIF_Image::frame(int frame_idx) {
  if (Fl::has_timeout(cb_animate, this) != 0) {
    Fl::warning("Fl_Anim_GIF_Image::frame(%d): not idle!\n", frame_idx);
  } else if ((frame_idx >= 0) && (frame_idx < frames())) {
    set_frame(frame_idx);
  } else {
    Fl::warning("Fl_Anim_GIF_Image::frame(%d): out of range!\n", frame_idx);
  }
}

int Fl_Anim_GIF_Image::frame_count(const char *name, const unsigned char *imgdata, size_t imglength) {
  Fl_Anim_GIF_Image temp;
  (void)temp.load(name, imgdata, imglength);
  const int frame_cnt = temp.valid() ? temp.frames() : 0;
  return frame_cnt;
}

int Fl_Anim_GIF_Image::frame_x(const int frame_idx) const {
  int res = -1;
  if ((frame_idx >= 0) && (frame_idx < frames())) {
    res = static_cast<int>(fi_->frames[static_cast<size_t>(frame_idx)].x());
  }
  return res;
}

int Fl_Anim_GIF_Image::frame_y(const int frame_idx) const {
  int res = -1;
  if ((frame_idx >= 0) && (frame_idx < frames())) {
    res = static_cast<int>(fi_->frames[static_cast<size_t>(frame_idx)].y());
  }
  return res;
}

int Fl_Anim_GIF_Image::frame_w(const int frame_idx) const {
  int res = -1;
  if ((frame_idx >= 0) && (frame_idx < frames())) {
    res = static_cast<int>(fi_->frames[static_cast<size_t>(frame_idx)].w());
  }
  return res;
}

int Fl_Anim_GIF_Image::frame_h(const int frame_idx) const {
  int res = -1;
  if ((frame_idx >= 0) && (frame_idx < frames())) {
    res = static_cast<int>(fi_->frames[static_cast<size_t>(frame_idx)].h());
  }
  return res;
}

void Fl_Anim_GIF_Image::frame_uncache(bool uncache) {
  uncache_ = uncache;
}

bool Fl_Anim_GIF_Image::frame_uncache() const {
  return uncache_;
}

int Fl_Anim_GIF_Image::frames() const {
  return static_cast<int>(fi_->frames.size());
}

Fl_Image *Fl_Anim_GIF_Image::image() const {
  Fl_Image *res = nullptr;
  if ((frame_ >= 0) && (frame_ < frames())) {
    res = fi_->frames[static_cast<size_t>(frame_)].rgb();
  }
  return res;
}

Fl_Image *Fl_Anim_GIF_Image::image(const int frame_idx) const {
  Fl_Image *res = nullptr;
  if ((frame_idx >= 0) && (frame_idx < frames())) {
    res = fi_->frames[static_cast<size_t>(frame_idx)].rgb();
  }
  return res;
}

bool Fl_Anim_GIF_Image::is_animated() const {
  return valid_ && (fi_->frames.size() > 1U);
}

bool Fl_GIF_Image::is_animated(const char *name) {
  return Fl_Anim_GIF_Image::frame_count(name, nullptr, 0U) > 1;
}

bool Fl_Anim_GIF_Image::load(const char *name, const unsigned char *imgdata, size_t imglength) {
  clear_frames();
  if (name_ != name) {
    std::free(name_);
    if (name != nullptr) {
      name_ = fl_strdup(name);
    } else {
      name_ = nullptr;
    }
  }

  uncache();
  if (alloc_data != 0) {
    for (int i = 0; i < count(); ++i) {
      delete[] const_cast<char*>(data()[i]);
    }
    delete[] const_cast<char**>(data());
  }
  alloc_data = 0;
  w(0);
  h(0);

  if ((name_ != nullptr) || (imgdata != nullptr)) {
    (void)fi_->load(name, imgdata, imglength);
  }

  frame_ = static_cast<int>(fi_->frames.size()) - 1;
  valid_ = fi_->valid;

  if (!valid_) {
    Fl::error("Fl_Anim_GIF_Image: %s has invalid format.\n", name_);
    ld(ERR_FORMAT);
  }
  return valid_;
}

const char *Fl_Anim_GIF_Image::name() const {
  return name_;
}

bool Fl_Anim_GIF_Image::next_frame() {
  bool ok = true;
  int cur_frame = frame_ + 1;
  const auto total_frames = static_cast<int>(fi_->frames.size());

  if (cur_frame >= total_frames) {
    fi_->loop++;
    if (Fl_Anim_GIF_Image::loop && (fi_->loop_count > 0) && (fi_->loop > fi_->loop_count)) {
      (void)stop();
    } else {
      cur_frame = 0;
    }
  }

  if (cur_frame >= total_frames) {
    ok = false;
  } else {
    set_frame(cur_frame);
    double cur_delay = fi_->frames[static_cast<size_t>(cur_frame)].delay();
    if ((min_delay > 0.0) && (cur_delay < min_delay)) {
      cur_delay = min_delay;
    }

    if (is_animated() && (cur_delay > 0.0) && (speed_ > 0.0)) {
      cur_delay /= speed_;
      Fl::add_timeout(cur_delay, cb_animate, this);
    }
  }
  return ok;
}

void Fl_Anim_GIF_Image::on_frame_data(GIF_FRAME &gf) {
  fi_->on_frame_data(gf);
}

void Fl_Anim_GIF_Image::on_extension_data(GIF_FRAME &gf) {
  fi_->on_extension_data(gf);
}

Fl_Anim_GIF_Image &Fl_Anim_GIF_Image::resize(const int w_val, const int h_val) {
  int new_w = w_val;
  int new_h = h_val;

  if ((canvas_ != nullptr) && (new_w == 0) && (new_h == 0)) {
    new_w = canvas_->w();
    new_h = canvas_->h();
  }
  if ((new_w != 0) && (new_h != 0) && ((new_w != this->w()) || (new_h != this->h()))) {
    fi_->resize(new_w, new_h);
    scale_frame();
    this->w(fi_->canvas_w);
    this->h(fi_->canvas_h);

    const auto dont_resize = static_cast<unsigned short>(flags_ & static_cast<unsigned short>(DONT_RESIZE_CANVAS));
    if ((canvas_ != nullptr) && (dont_resize == 0U)) {
      canvas_->size(this->w(), this->h());
    }
  }
  return *this;
}

Fl_Anim_GIF_Image &Fl_Anim_GIF_Image::resize(double scale_factor) {
  const int rw = static_cast<int>(std::round(static_cast<double>(w()) * scale_factor));
  const int rh = static_cast<int>(std::round(static_cast<double>(h()) * scale_factor));
  return resize(rw, rh);
}

void Fl_Anim_GIF_Image::scale_frame() const {
  const int i = frame_;
  if (i >= 0) {
    fi_->scale_frame(i);
  }
}

void Fl_Anim_GIF_Image::set_frame() const {
  const int i = frame_;
  if (i >= 0) {
    fi_->set_frame(i);
  }
}

void Fl_Anim_GIF_Image::set_frame(int frame_idx) {
  frame_ = frame_idx;
  if (uncache_ && (this->image() != nullptr)) {
    this->image()->uncache();
  }

  fi_->set_frame(frame_);

  Fl_Widget *cv = canvas();
  if (cv != nullptr) {
    Fl_Group *parent = cv->parent();
    const bool no_bg = !Fl::box_bg(cv->box());
    const bool outside = (((cv->align() & FL_ALIGN_INSIDE) == 0U) &&
                          ((cv->align() & FL_ALIGN_POSITION_MASK) != FL_ALIGN_CENTER));
    if ((parent != nullptr) && (no_bg || outside)) {
      parent->redraw();
    } else {
      cv->redraw();
    }
  }
}

double Fl_Anim_GIF_Image::speed() const {
  return speed_;
}

void Fl_Anim_GIF_Image::speed(const double speed_factor) {
  speed_ = speed_factor;
}

bool Fl_Anim_GIF_Image::start() {
  Fl::remove_timeout(cb_animate, this);
  if (!fi_->frames.empty()) {
    (void)next_frame();
  }
  return !fi_->frames.empty();
}

bool Fl_Anim_GIF_Image::stop() {
  Fl::remove_timeout(cb_animate, this);
  return !fi_->frames.empty();
}

bool Fl_Anim_GIF_Image::next() {
  if (!fi_->frames.empty() && (Fl::has_timeout(cb_animate, this) == 0)) {
    int f = frame() + 1;
    if (f >= frames()) {
      f = 0;
    }
    frame(f);
  }
  return !fi_->frames.empty();
}

void Fl_Anim_GIF_Image::uncache() {
  Fl_GIF_Image::uncache();
  for (auto &f : fi_->frames) {
    if (f.rgb() != nullptr) {
      f.rgb()->uncache();
    }
  }
}

bool Fl_Anim_GIF_Image::valid() const {
  return valid_;
}