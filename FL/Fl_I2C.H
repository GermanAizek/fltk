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

#ifndef Fl_I2C_H
#define Fl_I2C_H

#include "Fl_Export.H"
#include <stdint.h>

/**
  Cross-platform I2C device wrapper.
  
  This class provides a uniform interface to read and write to I2C devices.
  On Linux, it utilizes the standard `/dev/i2c-*` character devices and `ioctl`
  system calls for I2C and SMBus operations.
  
  On other platforms (Windows, macOS), the methods act as stubs and return
  error codes (-1) when operations fail, allowing cross-platform
  compilation without conditional logic in user code.
*/
class FL_EXPORT Fl_I2C {
  int fd_;
  int slave_addr_;

public:
  /**
    Initializes an I2C instance but does not open a port.
  */
  Fl_I2C();
  
  /**
    Cleans up the I2C instance, closing the device if open.
  */
  ~Fl_I2C();

  /**
    Opens an I2C device.
    \param device The path to the device, e.g., "/dev/i2c-1".
    \return 0 on success, or -1 on error.
  */
  int open(const char* device);
  
  /**
    Closes the currently open I2C device.
    \return 0 on success, or -1 if no device is open.
  */
  int close();

  /**
    Checks if an I2C device is currently open.
    \return 1 if open, 0 if not.
  */
  int is_open() const;

  /**
    Sets the I2C slave address for subsequent operations.
    \param addr The 7-bit or 10-bit slave address.
    \return 0 on success, or -1 on error.
  */
  int set_slave(int addr);

  /**
    Writes a single byte to the I2C device (without a register address).
    \param data The byte to write.
    \return 0 on success, or -1 on error.
  */
  int write_byte(uint8_t data);
  
  /**
    Reads a single byte from the I2C device (without a register address).
    \param data Reference to store the read byte.
    \return 0 on success, or -1 on error.
  */
  int read_byte(uint8_t& data);

  /**
    Writes multiple bytes to the I2C device.
    \param data Pointer to the buffer containing data.
    \param len Number of bytes to write.
    \return The number of bytes written, or -1 on error.
  */
  int write_data(const uint8_t* data, int len);
  
  /**
    Reads multiple bytes from the I2C device.
    \param data Pointer to the buffer to store read data.
    \param len Number of bytes to read.
    \return The number of bytes read, or -1 on error.
  */
  int read_data(uint8_t* data, int len);

  /**
    SMBus operation: Writes a byte to a specific register address.
    \param reg The register address.
    \param data The data byte to write.
    \return 0 on success, or -1 on error.
  */
  int write_reg_byte(uint8_t reg, uint8_t data);
  
  /**
    SMBus operation: Reads a byte from a specific register address.
    \param reg The register address.
    \param data Reference to store the read byte.
    \return 0 on success, or -1 on error.
  */
  int read_reg_byte(uint8_t reg, uint8_t& data);
};

#endif
