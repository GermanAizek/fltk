//
// PWM class for the Fast Light Tool Kit (FLTK).
//
// Copyright 2026 by Herman Semenoff (GermanAizek)
//

#include <FL/Fl_PWM.H>

#if defined(__linux__)
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

Fl_PWM::Fl_PWM(int chip, int channel) : chip_(chip), channel_(channel) {
  export_channel();
}

Fl_PWM::~Fl_PWM() {
  unexport_channel();
}

int Fl_PWM::export_channel() {
  char path[64];
  snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/export", chip_);
  int export_fd = ::open(path, O_WRONLY);
  if (export_fd < 0) return -1;
  char buf[32];
  snprintf(buf, sizeof(buf), "%d", channel_);
  ssize_t res = ::write(export_fd, buf, strlen(buf));
  ::close(export_fd);
  return (res > 0) ? 0 : -1;
}

int Fl_PWM::unexport_channel() {
  char path[64];
  snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/unexport", chip_);
  int unexport_fd = ::open(path, O_WRONLY);
  if (unexport_fd < 0) return -1;
  char buf[32];
  snprintf(buf, sizeof(buf), "%d", channel_);
  ssize_t res = ::write(unexport_fd, buf, strlen(buf));
  ::close(unexport_fd);
  return (res > 0) ? 0 : -1;
}

int Fl_PWM::period(long p) {
  char path[128];
  snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/pwm%d/period", chip_, channel_);
  int fd = ::open(path, O_WRONLY);
  if (fd < 0) return -1;
  char buf[32];
  snprintf(buf, sizeof(buf), "%ld", p);
  ssize_t res = ::write(fd, buf, strlen(buf));
  ::close(fd);
  return (res > 0) ? 0 : -1;
}

long Fl_PWM::period() const {
  char path[128];
  snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/pwm%d/period", chip_, channel_);
  int fd = ::open(path, O_RDONLY);
  if (fd < 0) return -1;
  char buf[32];
  ssize_t res = ::read(fd, buf, sizeof(buf) - 1);
  ::close(fd);
  if (res > 0) {
    buf[res] = '\0';
    return strtol(buf, NULL, 10);
  }
  return -1;
}

int Fl_PWM::duty_cycle(long d) {
  char path[128];
  snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", chip_, channel_);
  int fd = ::open(path, O_WRONLY);
  if (fd < 0) return -1;
  char buf[32];
  snprintf(buf, sizeof(buf), "%ld", d);
  ssize_t res = ::write(fd, buf, strlen(buf));
  ::close(fd);
  return (res > 0) ? 0 : -1;
}

long Fl_PWM::duty_cycle() const {
  char path[128];
  snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", chip_, channel_);
  int fd = ::open(path, O_RDONLY);
  if (fd < 0) return -1;
  char buf[32];
  ssize_t res = ::read(fd, buf, sizeof(buf) - 1);
  ::close(fd);
  if (res > 0) {
    buf[res] = '\0';
    return strtol(buf, NULL, 10);
  }
  return -1;
}

int Fl_PWM::duty_cycle_percent(double percent) {
  if (percent < 0.0) percent = 0.0;
  if (percent > 1.0) percent = 1.0;
  long p = period();
  if (p < 0) return -1;
  long d = (long)(p * percent);
  return duty_cycle(d);
}

int Fl_PWM::enable(int e) {
  char path[128];
  snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/pwm%d/enable", chip_, channel_);
  int fd = ::open(path, O_WRONLY);
  if (fd < 0) return -1;
  const char* val = e ? "1" : "0";
  ssize_t res = ::write(fd, val, 1);
  ::close(fd);
  return (res > 0) ? 0 : -1;
}

int Fl_PWM::enable() const {
  char path[128];
  snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/pwm%d/enable", chip_, channel_);
  int fd = ::open(path, O_RDONLY);
  if (fd < 0) return -1;
  char buf[4];
  ssize_t res = ::read(fd, buf, sizeof(buf) - 1);
  ::close(fd);
  if (res > 0) {
    return (buf[0] == '1') ? 1 : 0;
  }
  return -1;
}

#else
// Fallback for non-Linux platforms

Fl_PWM::Fl_PWM(int chip, int channel) : chip_(chip), channel_(channel) {
}

Fl_PWM::~Fl_PWM() {
}

int Fl_PWM::export_channel() { return -1; }
int Fl_PWM::unexport_channel() { return -1; }
int Fl_PWM::period(long) { return -1; }
long Fl_PWM::period() const { return -1; }
int Fl_PWM::duty_cycle(long) { return -1; }
long Fl_PWM::duty_cycle() const { return -1; }
int Fl_PWM::duty_cycle_percent(double) { return -1; }
int Fl_PWM::enable(int) { return -1; }
int Fl_PWM::enable() const { return -1; }

#endif
