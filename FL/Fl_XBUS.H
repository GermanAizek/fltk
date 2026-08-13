//
// XBUS Protocol class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_XBUS_H
#define Fl_XBUS_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>

class Fl_XBUS;
typedef void (*Fl_XBUS_Callback)(Fl_XBUS*, void*);

/**
  Cross-platform JR XBUS (Mode B / SRXL) protocol wrapper.
  
  Inherits from Fl_Serial_Port to provide decoding of XBUS RC frames.
  The standard Mode B protocol operates at 115,200 baud, 8 data bits, 
  no parity, and 1 stop bit (8N1).
  
  This class buffers bytes to parse the fixed 35-byte frames and validates
  the 16-bit CRC checksum. A callback is triggered when a valid frame of 
  type 0xA1 (16 RC Channels) is successfully decoded.
*/
class FL_EXPORT Fl_XBUS : public Fl_Serial_Port {
private:
  uint16_t channels_[16]; // 16 proportional channels (16-bit)
  
  uint8_t buf_idx_;
  uint8_t buffer_[35]; // XBUS frames (SRXL format 0xA1) are 35 bytes
  
  Fl_XBUS_Callback xbus_cb_;
  void* xbus_user_data_;

  // Internal serial callback to feed the parser
  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);
  void decode_frame();
  uint16_t calc_crc16(const uint8_t* data, int len);

public:
  Fl_XBUS();
  virtual ~Fl_XBUS();

  /**
    Opens the serial port and automatically configures it for XBUS
    (115,200 baud, 8 data bits, NO parity, 1 stop bit).
    \param port_name The path or name of the port.
    \return 0 on success, or -1 on error.
  */
  int open(const char* port_name);

  /**
    Registers a callback function to be called when a valid XBUS frame is received.
    \param cb The callback function.
    \param user_data Optional user data.
  */
  void xbus_callback(Fl_XBUS_Callback cb, void* user_data = nullptr);

  /**
    Gets the value of a specific channel.
    \param ch The channel number (1 to 16).
    \return The 16-bit value for the channel.
  */
  uint16_t channel(int ch) const;
  
  // For unit testing: manually feed a byte into the parser
  void feed_byte(uint8_t b) { process_byte(b); }
};

#endif
