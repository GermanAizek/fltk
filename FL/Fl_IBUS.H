//
// IBUS Protocol class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_IBUS_H
#define Fl_IBUS_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>

class Fl_IBUS;
typedef void (*Fl_IBUS_Callback)(Fl_IBUS*, void*);

/**
  Cross-platform IBUS (Flysky) protocol wrapper.
  
  Inherits from Fl_Serial_Port to provide decoding of IBUS RC frames.
  The standard protocol operates at 115,200 baud, 8 data bits, no parity, 
  and 1 stop bit (8N1).
  
  This class buffers bytes to parse the fixed 32-byte frames and validates
  the 16-bit checksum. A callback is triggered when a valid frame of type 0x40
  (RC Channels) is successfully decoded.
*/
class FL_EXPORT Fl_IBUS : public Fl_Serial_Port {
private:
  uint16_t channels_[14]; // 14 proportional channels (16-bit, usually 1000-2000)
  
  uint8_t buf_idx_;
  uint8_t buffer_[32]; // IBUS frames are always 32 bytes
  
  Fl_IBUS_Callback ibus_cb_;
  void* ibus_user_data_;

  // Internal serial callback to feed the parser
  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);
  void decode_frame();

public:
  Fl_IBUS();
  virtual ~Fl_IBUS();

  /**
    Opens the serial port and automatically configures it for IBUS
    (115,200 baud, 8 data bits, NO parity, 1 stop bit).
    \param port_name The path or name of the port.
    \return 0 on success, or -1 on error.
  */
  int open(const char* port_name);

  /**
    Registers a callback function to be called when a valid IBUS frame is received.
    \param cb The callback function.
    \param user_data Optional user data.
  */
  void ibus_callback(Fl_IBUS_Callback cb, void* user_data = nullptr);

  /**
    Gets the value of a specific channel.
    \param ch The channel number (1 to 14).
    \return The 16-bit value (typically 1000-2000) for the channel.
  */
  uint16_t channel(int ch) const;
  
  // For unit testing: manually feed a byte into the parser
  void feed_byte(uint8_t b) { process_byte(b); }
};

#endif
