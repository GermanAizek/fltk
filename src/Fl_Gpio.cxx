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

Fl_Gpio::Fl_Gpio(int pin) : pin_(pin), fd_(-1), dir_(IN), edge_(NONE), cb_(0), data_(0) {
  export_pin();
}

Fl_Gpio::~Fl_Gpio() {
  if (fd_ >= 0) {
    Fl::remove_fd(fd_);
    close(fd_);
    fd_ = -1;
  }
  unexport_pin();
}

int Fl_Gpio::export_pin() {
  int export_fd = ::open("/sys/class/gpio/export", O_WRONLY);
  if (export_fd < 0) return -1;
  char buf[32];
  snprintf(buf, sizeof(buf), "%d", pin_);
  ssize_t res = ::write(export_fd, buf, strlen(buf));
  ::close(export_fd);
  return (res > 0) ? 0 : -1;
}

int Fl_Gpio::unexport_pin() {
  int unexport_fd = ::open("/sys/class/gpio/unexport", O_WRONLY);
  if (unexport_fd < 0) return -1;
  char buf[32];
  snprintf(buf, sizeof(buf), "%d", pin_);
  ssize_t res = ::write(unexport_fd, buf, strlen(buf));
  ::close(unexport_fd);
  return (res > 0) ? 0 : -1;
}

int Fl_Gpio::direction(Direction d) {
  char path[64];
  snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin_);
  int dir_fd = ::open(path, O_WRONLY);
  if (dir_fd < 0) return -1;
  const char* val = (d == OUT) ? "out" : "in";
  ssize_t res = ::write(dir_fd, val, strlen(val));
  ::close(dir_fd);
  if (res > 0) {
    dir_ = d;
    return 0;
  }
  return -1;
}

int Fl_Gpio::value(int v) {
  if (dir_ != OUT) return -1;
  char path[64];
  snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin_);
  int val_fd = ::open(path, O_WRONLY);
  if (val_fd < 0) return -1;
  const char* str = (v) ? "1" : "0";
  ssize_t res = ::write(val_fd, str, 1);
  ::close(val_fd);
  return (res > 0) ? 0 : -1;
}

int Fl_Gpio::value() const {
  char path[64];
  snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin_);
  int val_fd = ::open(path, O_RDONLY);
  if (val_fd < 0) return -1;
  char ch;
  ssize_t res = ::read(val_fd, &ch, 1);
  ::close(val_fd);
  if (res > 0) {
    return (ch == '1') ? 1 : 0;
  }
  return -1;
}

int Fl_Gpio::edge(Edge e) {
  char path[64];
  snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/edge", pin_);
  int edge_fd = ::open(path, O_WRONLY);
  if (edge_fd < 0) return -1;
  const char* val = "none";
  switch (e) {
    case RISING: val = "rising"; break;
    case FALLING: val = "falling"; break;
    case BOTH: val = "both"; break;
    default: val = "none"; break;
  }
  ssize_t res = ::write(edge_fd, val, strlen(val));
  ::close(edge_fd);
  if (res > 0) {
    edge_ = e;
    if (e != NONE && fd_ < 0) {
      snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin_);
      fd_ = ::open(path, O_RDONLY | O_NONBLOCK);
      if (fd_ >= 0) {
        char tmp[4];
        ::read(fd_, tmp, sizeof(tmp)); // clear initial state
        Fl::add_fd(fd_, FL_EXCEPT, fd_callback, this);
      }
    } else if (e == NONE && fd_ >= 0) {
      Fl::remove_fd(fd_);
      ::close(fd_);
      fd_ = -1;
    }
    return 0;
  }
  return -1;
}

void Fl_Gpio::fd_callback(int fd, void* data) {
  Fl_Gpio* gpio = (Fl_Gpio*)data;
  if (gpio->fd_ == fd) {
    char tmp[4];
    ::lseek(fd, 0, SEEK_SET);
    ::read(fd, tmp, sizeof(tmp)); // consume the event
    gpio->do_callback();
  }
}

void Fl_Gpio::callback(void (*cb)(Fl_Gpio*, void*), void* data) {
  cb_ = cb;
  data_ = data;
}

void Fl_Gpio::do_callback() {
  if (cb_) {
    cb_(this, data_);
  }
}

#else
// Fallback for non-Linux platforms

Fl_Gpio::Fl_Gpio(int pin) : pin_(pin), fd_(-1), dir_(IN), edge_(NONE), cb_(0), data_(0) {
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
  if (cb_) cb_(this, data_);
}

#endif
