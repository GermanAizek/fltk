//
// I2C class for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_I2C.H>

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

// Internal helper for SMBus access via ioctl to avoid dependency on libi2c
static inline int fl_i2c_smbus_access(int file, char read_write, uint8_t command,
                                      int size, union i2c_smbus_data *data)
{
    struct i2c_smbus_ioctl_data args;
    args.read_write = read_write;
    args.command = command;
    args.size = size;
    args.data = data;
    return ioctl(file, I2C_SMBUS, &args);
}
#endif

Fl_I2C::Fl_I2C() : fd_(-1), slave_addr_(-1) {}

Fl_I2C::~Fl_I2C() {
  close();
}

int Fl_I2C::open(const char* device) {
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

int Fl_I2C::close() {
  if (fd_ >= 0) {
#ifdef __linux__
    ::close(fd_);
#endif
    fd_ = -1;
    slave_addr_ = -1;
    return 0;
  }
  return -1;
}

int Fl_I2C::is_open() const {
  return fd_ >= 0;
}

int Fl_I2C::set_slave(int addr) {
  if (fd_ < 0) return -1;
#ifdef __linux__
  if (ioctl(fd_, I2C_SLAVE, addr) < 0) {
    return -1;
  }
  slave_addr_ = addr;
  return 0;
#else
  (void)addr;
  return -1;
#endif
}

int Fl_I2C::write_byte(uint8_t data) {
  if (fd_ < 0 || slave_addr_ < 0) return -1;
#ifdef __linux__
  return (::write(fd_, &data, 1) == 1) ? 0 : -1;
#else
  (void)data;
  return -1;
#endif
}

int Fl_I2C::read_byte(uint8_t& data) {
  if (fd_ < 0 || slave_addr_ < 0) return -1;
#ifdef __linux__
  return (::read(fd_, &data, 1) == 1) ? 0 : -1;
#else
  (void)data;
  return -1;
#endif
}

int Fl_I2C::write_data(const uint8_t* data, int len) {
  if (fd_ < 0 || slave_addr_ < 0 || len < 0) return -1;
#ifdef __linux__
  return ::write(fd_, data, len);
#else
  (void)data;
  (void)len;
  return -1;
#endif
}

int Fl_I2C::read_data(uint8_t* data, int len) {
  if (fd_ < 0 || slave_addr_ < 0 || len < 0) return -1;
#ifdef __linux__
  return ::read(fd_, data, len);
#else
  (void)data;
  (void)len;
  return -1;
#endif
}

int Fl_I2C::write_reg_byte(uint8_t reg, uint8_t data) {
  if (fd_ < 0 || slave_addr_ < 0) return -1;
#ifdef __linux__
  union i2c_smbus_data args;
  args.byte = data;
  return fl_i2c_smbus_access(fd_, I2C_SMBUS_WRITE, reg, I2C_SMBUS_BYTE_DATA, &args) < 0 ? -1 : 0;
#else
  (void)reg;
  (void)data;
  return -1;
#endif
}

int Fl_I2C::read_reg_byte(uint8_t reg, uint8_t& data) {
  if (fd_ < 0 || slave_addr_ < 0) return -1;
#ifdef __linux__
  union i2c_smbus_data args;
  if (fl_i2c_smbus_access(fd_, I2C_SMBUS_READ, reg, I2C_SMBUS_BYTE_DATA, &args) < 0) {
    return -1;
  }
  data = args.byte & 0xFF;
  return 0;
#else
  (void)reg;
  (void)data;
  return -1;
#endif
}
