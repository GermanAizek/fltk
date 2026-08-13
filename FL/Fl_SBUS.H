//
// SBUS Protocol class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_SBUS_H
#define Fl_SBUS_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>

class Fl_SBUS;
typedef void (*Fl_SBUS_Callback)(Fl_SBUS*, void*);

/**
  Cross-platform SBUS (Serial BUS) protocol wrapper.
  
  Inherits from Fl_Serial_Port to provide decoding of the 25-byte SBUS frames
  used by Futaba and FrSky receivers. The protocol operates at 100,000 baud, 
  8 data bits, Even parity, and 2 stop bits (8E2).
  
  The parser maintains an internal buffer and triggers a user-defined callback
  whenever a valid complete frame is successfully parsed.
*/
class FL_EXPORT Fl_SBUS : public Fl_Serial_Port {
private:
  uint16_t channels_[18]; // 16 proportional channels (11-bit) + 2 digital channels
  bool failsafe_;
  bool frame_lost_;
  
  uint8_t buffer_[25];
  int buf_idx_;
  
  Fl_SBUS_Callback sbus_cb_;
  void* sbus_user_data_;

  // Internal serial callback to feed the parser
  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);
  void decode_frame();

public:
  Fl_SBUS();
  virtual ~Fl_SBUS();

  /**
    Opens the serial port and automatically configures it for SBUS
    (100,000 baud, 8 data bits, EVEN parity, 2 stop bits).
    \param port_name The path or name of the port.
    \return 0 on success, or -1 on error.
  */
  int open(const char* port_name);

  /**
    Registers a callback function to be called when a valid 25-byte SBUS frame is received.
    \param cb The callback function.
    \param user_data Optional user data.
  */
  void sbus_callback(Fl_SBUS_Callback cb, void* user_data = nullptr);

  /**
    Gets the value of a specific channel.
    \param ch The channel number (1 to 18).
    \return The 11-bit value (0-2047) for proportional channels (1-16), or 0/1 for digital channels (17, 18).
  */
  uint16_t channel(int ch) const;
  
  /**
    Gets the current failsafe status flag.
    \return true if failsafe is active.
  */
  bool failsafe() const { return failsafe_; }
  
  /**
    Gets the current frame lost status flag.
    \return true if a frame was lost.
  */
  bool frame_lost() const { return frame_lost_; }
  
  // For unit testing: manually feed a byte into the parser
  void feed_byte(uint8_t b) { process_byte(b); }
};

#endif
