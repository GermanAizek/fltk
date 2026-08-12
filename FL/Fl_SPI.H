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

#ifndef Fl_SPI_H
#define Fl_SPI_H

#include "Fl_Export.H"
#include <stdint.h>

/**
  Cross-platform SPI device wrapper.
  
  This class provides a uniform interface to read and write to SPI devices.
  On Linux, it utilizes the standard `/dev/spidev*` character devices and `ioctl`
  system calls for SPI operations.
  
  On other platforms (Windows, macOS), the methods act as stubs and return
  error codes (-1) when operations fail, allowing cross-platform
  compilation without conditional logic in user code.
*/
class FL_EXPORT Fl_SPI {
  int fd_;
  int mode_;
  int bits_;
  int speed_;

public:
  /**
    Initializes an SPI instance but does not open a port.
  */
  Fl_SPI();
  
  /**
    Cleans up the SPI instance, closing the device if open.
  */
  ~Fl_SPI();

  /**
    Opens an SPI device.
    \param device The path to the device, e.g., "/dev/spidev0.0".
    \return 0 on success, or -1 on error.
  */
  int open(const char* device);
  
  /**
    Closes the currently open SPI device.
    \return 0 on success, or -1 if no device is open.
  */
  int close();

  /**
    Checks if an SPI device is currently open.
    \return 1 if open, 0 if not.
  */
  int is_open() const;

  /**
    Sets the SPI mode (0, 1, 2, or 3).
    \param mode The SPI mode.
    \return 0 on success, or -1 on error.
  */
  int set_mode(int mode);

  /**
    Sets the number of bits per word for SPI transfers.
    \param bits Number of bits per word, typically 8.
    \return 0 on success, or -1 on error.
  */
  int set_bits_per_word(int bits);

  /**
    Sets the maximum SPI clock speed in Hz.
    \param speed Clock speed in Hz.
    \return 0 on success, or -1 on error.
  */
  int set_max_speed_hz(int speed);

  /**
    Performs a full-duplex SPI transfer.
    \param tx_buf Pointer to the buffer containing data to transmit.
    \param rx_buf Pointer to the buffer to store received data.
    \param len Number of bytes to transfer.
    \return The number of bytes transferred, or -1 on error.
  */
  int transfer(const uint8_t* tx_buf, uint8_t* rx_buf, int len);
};

#endif
