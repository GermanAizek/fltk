//
// CRSF Protocol class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_CRSF_H
#define Fl_CRSF_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>

class Fl_CRSF;
typedef void (*Fl_CRSF_Callback)(Fl_CRSF*, void*);

/**
  Cross-platform CRSF (Crossfire) protocol wrapper.
  
  Inherits from Fl_Serial_Port to provide decoding of CRSF telemetry and RC frames.
  The standard protocol operates at 420,000 baud, 8 data bits, no parity, 
  and 1 stop bit (8N1).
  
  This class maintains a state machine to parse variable-length frames and
  validate the CRC8. A callback is triggered when a valid frame of type 0x16 (RC Channels)
  or other valid frame is successfully decoded.
*/
class FL_EXPORT Fl_CRSF : public Fl_Serial_Port {
private:
  uint16_t channels_[16]; // 16 proportional channels (11-bit)
  
  // State machine for parsing
  enum State {
    WAIT_SYNC = 0,
    WAIT_LENGTH,
    WAIT_PAYLOAD
  };
  
  State state_;
  uint8_t current_len_;
  uint8_t buf_idx_;
  uint8_t buffer_[64]; // Max CRSF frame is 64 bytes
  
  Fl_CRSF_Callback crsf_cb_;
  void* crsf_user_data_;

  // Internal serial callback to feed the parser
  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);
  void decode_rc_channels();
  uint8_t calc_crc8(const uint8_t* data, int len);

public:
  Fl_CRSF();
  virtual ~Fl_CRSF();

  /**
    Opens the serial port and automatically configures it for CRSF
    (420,000 baud, 8 data bits, NO parity, 1 stop bit).
    \param port_name The path or name of the port.
    \return 0 on success, or -1 on error.
  */
  int open(const char* port_name);

  /**
    Registers a callback function to be called when a valid CRSF frame is received.
    \param cb The callback function.
    \param user_data Optional user data.
  */
  void crsf_callback(Fl_CRSF_Callback cb, void* user_data = nullptr);

  /**
    Gets the value of a specific channel.
    \param ch The channel number (1 to 16).
    \return The 11-bit value (0-2047) for proportional channels (1-16).
  */
  uint16_t channel(int ch) const;
  
  // For unit testing: manually feed a byte into the parser
  void feed_byte(uint8_t b) { process_byte(b); }
};

#endif
