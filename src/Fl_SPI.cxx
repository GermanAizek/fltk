//
// SPI class for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
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

#include <FL/Fl_SPI.H>

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <string.h>
#endif

Fl_SPI::Fl_SPI() : fd_(-1), mode_(0), bits_(8), speed_(500000) {}

Fl_SPI::~Fl_SPI() {
  close();
}

int Fl_SPI::open(const char* device) {
  close();
#ifdef __linux__
  fd_ = ::open(device, O_RDWR);
  if (fd_ < 0) return -1;
  return 0;
#else
  (void)device;
  return -1;
#endif
}

int Fl_SPI::close() {
  if (fd_ >= 0) {
#ifdef __linux__
    ::close(fd_);
#endif
    fd_ = -1;
    return 0;
  }
  return -1;
}

int Fl_SPI::is_open() const {
  return fd_ >= 0;
}

int Fl_SPI::set_mode(int mode) {
  if (fd_ < 0) return -1;
#ifdef __linux__
  uint8_t m = mode;
  if (ioctl(fd_, SPI_IOC_WR_MODE, &m) < 0) return -1;
  if (ioctl(fd_, SPI_IOC_RD_MODE, &m) < 0) return -1;
  mode_ = m;
  return 0;
#else
  (void)mode;
  return -1;
#endif
}

int Fl_SPI::set_bits_per_word(int bits) {
  if (fd_ < 0) return -1;
#ifdef __linux__
  uint8_t b = bits;
  if (ioctl(fd_, SPI_IOC_WR_BITS_PER_WORD, &b) < 0) return -1;
  if (ioctl(fd_, SPI_IOC_RD_BITS_PER_WORD, &b) < 0) return -1;
  bits_ = b;
  return 0;
#else
  (void)bits;
  return -1;
#endif
}

int Fl_SPI::set_max_speed_hz(int speed) {
  if (fd_ < 0) return -1;
#ifdef __linux__
  uint32_t s = speed;
  if (ioctl(fd_, SPI_IOC_WR_MAX_SPEED_HZ, &s) < 0) return -1;
  if (ioctl(fd_, SPI_IOC_RD_MAX_SPEED_HZ, &s) < 0) return -1;
  speed_ = s;
  return 0;
#else
  (void)speed;
  return -1;
#endif
}

int Fl_SPI::transfer(const uint8_t* tx_buf, uint8_t* rx_buf, int len) {
  if (fd_ < 0 || len < 0) return -1;
#ifdef __linux__
  struct spi_ioc_transfer tr;
  memset(&tr, 0, sizeof(tr));
  tr.tx_buf = (unsigned long)tx_buf;
  tr.rx_buf = (unsigned long)rx_buf;
  tr.len = len;
  tr.speed_hz = speed_;
  tr.bits_per_word = bits_;

  if (ioctl(fd_, SPI_IOC_MESSAGE(1), &tr) < 1) return -1;
  return len;
#else
  (void)tx_buf;
  (void)rx_buf;
  (void)len;
  return -1;
#endif
}
