//
// Serial Port class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Serial_Port_H
#define Fl_Serial_Port_H

#include "Fl_Export.H"
#include <stdint.h>

class Fl_Serial_Port;
typedef void (*Fl_Serial_Callback)(Fl_Serial_Port*, void*);

/**
  Cross-platform Serial Port (COM Port) wrapper.
  
  This class provides a uniform interface to configure, read, and write to serial ports.
  It supports common configuration parameters such as baud rate, data bits, parity, 
  and stop bits.
  
  On Windows, it utilizes the WinAPI (`CreateFile`, `GetCommState`, etc.).
  On POSIX systems (Linux, macOS), it utilizes standard `termios` functions.
*/
class FL_EXPORT Fl_Serial_Port {
public:
  enum DataBits {
    DATA_5 = 5,
    DATA_6 = 6,
    DATA_7 = 7,
    DATA_8 = 8
  };

  enum Parity {
    PARITY_NONE = 0,
    PARITY_ODD,
    PARITY_EVEN,
    PARITY_MARK,
    PARITY_SPACE
  };

  enum StopBits {
    STOP_1 = 0,
    STOP_1_5,
    STOP_2
  };

private:
  Fl_Serial_Callback callback_;
  void* user_data_;

#if defined(_WIN32)
  void* handle_; // HANDLE on Windows
  void* thread_data_; // Internal pointer for std::thread state
#else
  int fd_; // File descriptor on POSIX
#endif

public:
  /**
    Initializes a serial port instance but does not open a port.
  */
  Fl_Serial_Port();
  
  /**
    Cleans up the serial port instance, closing the device if open.
  */
  ~Fl_Serial_Port();

  /**
    Opens a serial port.
    \param port_name The path or name of the port, e.g., "COM1" or "/dev/ttyS0".
    \return 0 on success, or -1 on error.
  */
  int open(const char* port_name);
  
  /**
    Closes the currently open serial port.
    \return 0 on success, or -1 if no port is open.
  */
  int close();

  /**
    Checks if a serial port is currently open.
    \return 1 if open, 0 if not.
  */
  int is_open() const;

  /**
    Sets the baud rate for the open port.
    \param baud The baud rate (e.g., 9600, 115200).
    \return 0 on success, or -1 on error.
  */
  int set_baud_rate(int baud);

  /**
    Sets the number of data bits.
    \param bits Data bits enum (e.g., DATA_8).
    \return 0 on success, or -1 on error.
  */
  int set_data_bits(DataBits bits);

  /**
    Sets the parity type.
    \param parity Parity enum (e.g., PARITY_NONE).
    \return 0 on success, or -1 on error.
  */
  int set_parity(Parity parity);

  /**
    Sets the number of stop bits.
    \param bits StopBits enum (e.g., STOP_1).
    \return 0 on success, or -1 on error.
  */
  int set_stop_bits(StopBits bits);

  /**
    Writes multiple bytes to the serial port.
    \param data Pointer to the buffer containing data.
    \param len Number of bytes to write.
    \return The number of bytes written, or -1 on error.
  */
  int write_data(const void* data, int len);
  
  /**
    Reads multiple bytes from the serial port.
    \param buffer Pointer to the buffer to store read data.
    \param len Maximum number of bytes to read.
    \return The number of bytes read, or -1 on error.
  */
  int read_data(void* buffer, int len);

  /**
    Sets the callback function to be called when data is available to read.
    This enables asynchronous I/O (like Qt's readyRead).
    \param cb The callback function.
    \param user_data Optional user data passed to the callback.
  */
  void callback(Fl_Serial_Callback cb, void* user_data = nullptr);
  
  /**
    Triggers the callback manually.
  */
  void do_callback();
  
  /**
    Gets the current callback.
  */
  Fl_Serial_Callback callback() const { return callback_; }
  
  /**
    Gets the current user data.
  */
  void* user_data() const { return user_data_; }
  
  /**
    Sets the user data.
  */
  void user_data(void* data) { user_data_ = data; }
};

#endif
