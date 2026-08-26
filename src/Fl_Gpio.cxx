//
// GPIO class for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by Herman Semenoff (GermanAizek)
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

#include <FL/Fl_Gpio.H>

#if defined(__linux__)
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

Fl_Gpio::Fl_Gpio(int pin) : pin_(pin), fd_(-1), dir_(IN), edge_(NONE), cb_(nullptr), data_(nullptr) {
  (void)export_pin();
}

Fl_Gpio::~Fl_Gpio() {
  if (fd_ >= 0) {
    Fl::remove_fd(fd_);
    (void)::close(fd_);
    fd_ = -1;
  }
  (void)unexport_pin();
}

int Fl_Gpio::export_pin() const {
  int ret = -1;
  const int export_fd = ::open("/sys/class/gpio/export", O_WRONLY);
  if (export_fd >= 0) {
    char buf[32] = {0};
    const int len = snprintf(&buf[0], sizeof(buf), "%d", pin_);
    if ((len > 0) && (static_cast<size_t>(len) < sizeof(buf))) {
      const ssize_t res = ::write(export_fd, &buf[0], static_cast<size_t>(len));
      if (res > 0) {
        ret = 0;
      }
    }
    (void)::close(export_fd);
  }
  return ret;
}

int Fl_Gpio::unexport_pin() const {
  int ret = -1;
  const int unexport_fd = ::open("/sys/class/gpio/unexport", O_WRONLY);
  if (unexport_fd >= 0) {
    char buf[32] = {0};
    const int len = snprintf(&buf[0], sizeof(buf), "%d", pin_);
    if ((len > 0) && (static_cast<size_t>(len) < sizeof(buf))) {
      const ssize_t res = ::write(unexport_fd, &buf[0], static_cast<size_t>(len));
      if (res > 0) {
        ret = 0;
      }
    }
    (void)::close(unexport_fd);
  }
  return ret;
}

int Fl_Gpio::direction(Direction d) {
  int ret = -1;
  char path[64] = {0};
  const int len = snprintf(&path[0], sizeof(path), "/sys/class/gpio/gpio%d/direction", pin_);
  if ((len > 0) && (static_cast<size_t>(len) < sizeof(path))) {
    const int dir_fd = ::open(&path[0], O_WRONLY);
    if (dir_fd >= 0) {
      const char* const val = (d == OUT) ? "out" : "in";
      const size_t val_len = (d == OUT) ? 3U : 2U;
      const ssize_t res = ::write(dir_fd, val, val_len);
      (void)::close(dir_fd);
      if (res > 0) {
        dir_ = d;
        ret = 0;
      }
    }
  }
  return ret;
}

int Fl_Gpio::value(int v) {
  int ret = -1;
  if (dir_ == OUT) {
    char path[64] = {0};
    const int len = snprintf(&path[0], sizeof(path), "/sys/class/gpio/gpio%d/value", pin_);
    if ((len > 0) && (static_cast<size_t>(len) < sizeof(path))) {
      const int val_fd = ::open(&path[0], O_WRONLY);
      if (val_fd >= 0) {
        const char* const str = (v != 0) ? "1" : "0";
        const ssize_t res = ::write(val_fd, str, 1U);
        (void)::close(val_fd);
        if (res > 0) {
          ret = 0;
        }
      }
    }
  }
  return ret;
}

int Fl_Gpio::value() const {
  int ret = -1;
  char path[64] = {0};
  const int len = snprintf(&path[0], sizeof(path), "/sys/class/gpio/gpio%d/value", pin_);
  if ((len > 0) && (static_cast<size_t>(len) < sizeof(path))) {
    const int val_fd = ::open(&path[0], O_RDONLY);
    if (val_fd >= 0) {
      char ch = '\0';
      const ssize_t res = ::read(val_fd, &ch, 1U);
      (void)::close(val_fd);
      if (res > 0) {
        ret = (ch == '1') ? 1 : 0;
      }
    }
  }
  return ret;
}

int Fl_Gpio::edge(Edge e) {
  int ret = -1;
  char path[64] = {0};
  const int len = snprintf(&path[0], sizeof(path), "/sys/class/gpio/gpio%d/edge", pin_);
  if ((len > 0) && (static_cast<size_t>(len) < sizeof(path))) {
    const int edge_fd = ::open(&path[0], O_WRONLY);
    if (edge_fd >= 0) {
      const char* val = "none";
      size_t val_len = 4U;
      switch (e) {
        case RISING:
          val = "rising";
          val_len = 6U;
          break;
        case FALLING:
          val = "falling";
          val_len = 7U;
          break;
        case BOTH:
          val = "both";
          val_len = 4U;
          break;
        default:
          break;
      }
      const ssize_t res = ::write(edge_fd, val, val_len);
      (void)::close(edge_fd);
      if (res > 0) {
        edge_ = e;
        if ((e != NONE) && (fd_ < 0)) {
          const int len2 = snprintf(&path[0], sizeof(path), "/sys/class/gpio/gpio%d/value", pin_);
          if ((len2 > 0) && (static_cast<size_t>(len2) < sizeof(path))) {
            fd_ = ::open(&path[0], O_RDONLY | O_NONBLOCK);
            if (fd_ >= 0) {
              char tmp[4] = {0};
              (void)::read(fd_, &tmp[0], sizeof(tmp)); // clear initial state
              Fl::add_fd(fd_, FL_EXCEPT, fd_callback, this);
            }
          }
        } else if ((e == NONE) && (fd_ >= 0)) {
          Fl::remove_fd(fd_);
          (void)::close(fd_);
          fd_ = -1;
        } else {
          // MISRA compliance: final else clause
        }
        ret = 0;
      }
    }
  }
  return ret;
}

void Fl_Gpio::fd_callback(int fd, void* data) {
  if (data != nullptr) {
    Fl_Gpio* const gpio = static_cast<Fl_Gpio*>(data);
    if (gpio->fd_ == fd) {
      char tmp[4] = {0};
      (void)::lseek(fd, 0, SEEK_SET);
      (void)::read(fd, &tmp[0], sizeof(tmp)); // consume the event
      gpio->do_callback();
    }
  }
}

void Fl_Gpio::callback(void (*cb)(Fl_Gpio*, void*), void* data) {
  cb_ = cb;
  data_ = data;
}

void Fl_Gpio::do_callback() {
  if (cb_ != nullptr) {
    cb_(this, data_);
  }
}

#else
// Fallback for non-Linux platforms

Fl_Gpio::Fl_Gpio(int pin) : pin_(pin), fd_(-1), dir_(IN), edge_(NONE), cb_(nullptr), data_(nullptr) {
}

Fl_Gpio::~Fl_Gpio() {
}

int Fl_Gpio::direction(Direction d) {
  dir_ = d;
  return -1;
}

int Fl_Gpio::value(int v) {
  return -1;
}

int Fl_Gpio::value() const {
  return -1;
}

int Fl_Gpio::edge(Edge e) {
  edge_ = e;
  return -1;
}

void Fl_Gpio::callback(void (*cb)(Fl_Gpio*, void*), void* data) {
  cb_ = cb;
  data_ = data;
}

void Fl_Gpio::do_callback() {
  if (cb_ != nullptr) {
    cb_(this, data_);
  }
}

#endif
