//
// Optional argument initialization code for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2026 by Bill Spitzak and others.
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
#include <FL/Fl_Window.H>
#include <FL/Fl_Tooltip.H>
#include "Fl_Window_Driver.H"
#include "Fl_System_Driver.H"
#include "Fl_Screen_Driver.H"
#include "flstring.h"

#include <string>

namespace {

class ArgContext {
private:
  bool m_arg_called{false};
  bool m_return_i{false};
  const char *m_name{nullptr};
  const char *m_geometry{nullptr};
  const char *m_title{nullptr};
  bool m_window_initialized{false};

public:
  ArgContext() = default;
  ~ArgContext() = default;

  ArgContext(const ArgContext &) = delete;
  ArgContext &operator=(const ArgContext &) = delete;
  ArgContext(ArgContext &&) = delete;
  ArgContext &operator=(ArgContext &&) = delete;

  static ArgContext &instance() {
    static ArgContext ctx;
    return ctx;
  }

  bool arg_called() const { return m_arg_called; }
  void set_arg_called(bool val) { m_arg_called = val; }

  bool return_i() const { return m_return_i; }
  void set_return_i(bool val) { m_return_i = val; }

  const char *name() const { return m_name; }
  void set_name(const char *val) { m_name = val; }

  const char *geometry() const { return m_geometry; }
  void set_geometry(const char *val) { m_geometry = val; }

  const char *title() const { return m_title; }
  void set_title(const char *val) { m_title = val; }

  bool window_initialized() const { return m_window_initialized; }
  void set_window_initialized(bool val) { m_window_initialized = val; }
};

bool fl_match(const char *a, const char *s, const int atleast = 1) {
  bool match = false;
  if ((a != nullptr) && (s != nullptr)) {
    const char *b = s;
    while ((*a != '\0') && (*b != '\0') && ((*a == *b) || (fl_ascii_tolower(*a) == *b))) {
      ++a;
      ++b;
    }
    match = (*a == '\0') && (b >= (s + atleast));
  }
  return match;
}

const char * const help_text =
"options are:\n"
" -bg2 color\n"
" -bg color\n"
" -di[splay] host:n.n\n"
" -dn[d]\n"
" -fg color\n"
" -g[eometry] WxH+X+Y\n"
" -i[conic]\n"
" -k[bd]\n"
" -na[me] classname\n"
" -nod[nd]\n"
" -nok[bd]\n"
" -not[ooltips]\n"
" -s[cheme] scheme\n"
" -ti[tle] windowtitle\n"
" -to[oltips]";

} // anonymous namespace

// These symbols reside in Fl_get_system_colors and are configured via arguments:
extern const char *fl_fg;
extern const char *fl_bg;
extern const char *fl_bg2;

int Fl::arg(const int argc, char** argv, int& i)
{
  int eaten = 0;
  ArgContext& ctx = ArgContext::instance();
  ctx.set_arg_called(true);

  if (argv != nullptr) {
    const char* const* const safe_argv = argv;
    const char* s = safe_argv[i];

    if (s == nullptr) {
      ++i;
      eaten = 1;
    } else if ((s[0] != '-') || (s[1] == '-') || (s[1] == '\0')) {
      ctx.set_return_i(true);
      eaten = 0;
    } else {
      ++s; // Point past dash

      if (fl_match(s, "iconic", 1)) {
        Fl_Window::show_next_window_iconic(1);
        ++i;
        eaten = 1;
      } else if (fl_match(s, "kbd", 1)) {
        Fl::visible_focus(1);
        ++i;
        eaten = 1;
      } else if (fl_match(s, "nokbd", 3)) {
        Fl::visible_focus(0);
        ++i;
        eaten = 1;
      } else if (fl_match(s, "dnd", 2)) {
        Fl::dnd_text_ops(1);
        ++i;
        eaten = 1;
      } else if (fl_match(s, "nodnd", 3)) {
        Fl::dnd_text_ops(0);
        ++i;
        eaten = 1;
      } else if (fl_match(s, "tooltips", 2)) {
        Fl_Tooltip::enable();
        ++i;
        eaten = 1;
      } else if (fl_match(s, "notooltips", 3)) {
        Fl_Tooltip::disable();
        ++i;
        eaten = 1;
      } else if (Fl::system_driver()->single_arg(s) != 0) {
        ++i;
        eaten = 1;
      } else if (i >= (argc - 1)) {
        eaten = 0;
      } else {
        const char* v = safe_argv[i + 1];
        if (v == nullptr) {
          eaten = 0;
        } else if (fl_match(s, "geometry", 1)) {
          int gx = 0;
          int gy = 0;
          unsigned int gw = 0U;
          unsigned int gh = 0U;
          const int flags = Fl::screen_driver()->XParseGeometry(v, &gx, &gy, &gw, &gh);
          if (flags != 0) {
            ctx.set_geometry(v);
            i += 2;
            eaten = 2;
          } else {
            eaten = 0;
          }
        } else if (fl_match(s, "display", 2)) {
          Fl::screen_driver()->display(v);
          i += 2;
          eaten = 2;
        } else if (Fl::system_driver()->arg_and_value(s, v) != 0) {
          i += 2;
          eaten = 2;
        } else if (fl_match(s, "title", 2)) {
          ctx.set_title(v);
          i += 2;
          eaten = 2;
        } else if (fl_match(s, "name", 2)) {
          ctx.set_name(v);
          i += 2;
          eaten = 2;
        } else if (fl_match(s, "bg2", 3) || fl_match(s, "background2", 11)) {
          fl_bg2 = v;
          i += 2;
          eaten = 2;
        } else if (fl_match(s, "bg", 2) || fl_match(s, "background", 10)) {
          fl_bg = v;
          i += 2;
          eaten = 2;
        } else if (fl_match(s, "fg", 2) || fl_match(s, "foreground", 10)) {
          fl_fg = v;
          i += 2;
          eaten = 2;
        } else if (fl_match(s, "scheme", 1)) {
          Fl::scheme(v);
          i += 2;
          eaten = 2;
        } else {
          eaten = 0;
        }
      }
    }
  }

  return eaten;
}

int Fl::args(int argc, char** argv, int& i, Fl_Args_Handler cb)
{
  int result = 0;
  ArgContext& ctx = ArgContext::instance();
  ctx.set_arg_called(true);
  i = 1; // Skip argv[0]

  while (i < argc) {
    if ((cb != nullptr) && (cb(argc, argv, i) != 0)) {
      continue;
    }
    if (arg(argc, argv, i) == 0) {
      result = ctx.return_i() ? i : 0;
      break;
    }
  }

  if (i >= argc) {
    result = i;
  }

  return result;
}

const char* const Fl::help = help_text + 13U;

void Fl_Window::show(int argc, char **argv) {
  ArgContext &ctx = ArgContext::instance();

  if ((argc != 0) && (!ctx.arg_called())) {
    (void)Fl::args(argc, argv);
  }

  Fl::get_system_colors();
  pWindowDriver->show_with_args_begin();

  if (!ctx.window_initialized()) {
    if (ctx.geometry() != nullptr) {
      int gx = x();
      int gy = y();
      const auto gw = static_cast<unsigned int>(w());
      const auto gh = static_cast<unsigned int>(h());
      unsigned int parsed_w = gw;
      unsigned int parsed_h = gh;

      const int fl = Fl::screen_driver()->XParseGeometry(ctx.geometry(), &gx, &gy, &parsed_w, &parsed_h);

      if ((static_cast<unsigned int>(fl) & static_cast<unsigned int>(Fl_Screen_Driver::fl_XNegative)) != 0U) {
        gx = Fl::w() - w() + gx;
      }
      if ((static_cast<unsigned int>(fl) & static_cast<unsigned int>(Fl_Screen_Driver::fl_YNegative)) != 0U) {
        gy = Fl::h() - h() + gy;
      }

      Fl_Widget *r = resizable();
      if (r == nullptr) {
        resizable(this);
      }

      const auto pos_mask = static_cast<unsigned int>(Fl_Screen_Driver::fl_XValue) |
                            static_cast<unsigned int>(Fl_Screen_Driver::fl_YValue);

      if ((static_cast<unsigned int>(fl) & pos_mask) != 0U) {
        x(-1);
        resize(gx, gy, static_cast<int>(parsed_w), static_cast<int>(parsed_h));
      } else {
        size(static_cast<int>(parsed_w), static_cast<int>(parsed_h));
      }
      resizable(r);
    }
  }

  if (ctx.name() != nullptr) {
    xclass(ctx.name());
    ctx.set_name(nullptr);
  } else if ((xclass() == nullptr) || (std::char_traits<char>::compare(xclass(), "FLTK", 4U) == 0)) {
    if ((argv != nullptr) && (argv[0] != nullptr)) {
      xclass(fl_filename_name(argv[0]));
    }
  } else {
    // Keep custom xclass
  }

  if (ctx.title() != nullptr) {
    label(ctx.title());
    ctx.set_title(nullptr);
  } else if (label() == nullptr) {
    label(xclass());
  } else {
    // Keep custom label
  }

  if (!ctx.window_initialized()) {
    ctx.set_window_initialized(true);
    (void)Fl::scheme(Fl::scheme());
  }

  show();
  pWindowDriver->show_with_args_end(argc, argv);
}

void Fl::args(int argc, char **argv) {
  int i = 0;
  if (Fl::args(argc, argv, i) < argc) {
    Fl::error("%s", help_text);
  }
}