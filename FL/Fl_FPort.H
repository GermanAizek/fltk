//
// FPort Protocol class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_FPort_H
#define Fl_FPort_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>

class Fl_FPort;
typedef void (*Fl_FPort_Callback)(Fl_FPort*, void*);

/**
  Cross-platform FrSky FPort protocol wrapper.
  
  Inherits from Fl_Serial_Port to provide decoding of FPort frames.
  The FPort protocol operates at 115,200 baud, 8 data bits, no parity, 
  and 1 stop bit (8N1), generally in a half-duplex configuration.
  
  This class manages byte stuffing (escaping 0x7E and 0x7D), unpacks the 
  11-bit SBUS style channels embedded inside, and validates the standard 
  FrSky checksum.
*/
class FL_EXPORT Fl_FPort : public Fl_Serial_Port {
private:
  uint16_t channels_[16]; // 16 proportional channels (11-bit)
  uint8_t flags_;
  uint8_t rssi_;
  
  enum State {
    WAIT_SYNC = 0,
    WAIT_LENGTH,
    WAIT_TYPE,
    WAIT_PAYLOAD
  };
  
  State state_;
  bool escape_next_;
  
  uint8_t frame_length_;
  uint8_t frame_type_;
  
  uint8_t buf_idx_;
  uint8_t buffer_[128]; 
  
  Fl_FPort_Callback fport_cb_;
  void* fport_user_data_;

  // Internal serial callback to feed the parser
  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);
  void decode_rc_frame();

public:
  Fl_FPort();
  virtual ~Fl_FPort();

  /**
    Opens the serial port and automatically configures it for FPort
    (115,200 baud, 8 data bits, NO parity, 1 stop bit).
    \param port_name The path or name of the port.
    \return 0 on success, or -1 on error.
  */
  int open(const char* port_name);

  /**
    Registers a callback function to be called when a valid FPort frame is received.
    \param cb The callback function.
    \param user_data Optional user data.
  */
  void fport_callback(Fl_FPort_Callback cb, void* user_data = nullptr);

  /**
    Gets the value of a specific channel.
    \param ch The channel number (1 to 16).
    \return The 11-bit value for the channel (typically 172 to 1811).
  */
  uint16_t channel(int ch) const;
  
  /**
    Gets the FPort / SBUS flags (e.g., failsafe, frame lost).
    \return The flags byte.
  */
  uint8_t flags() const { return flags_; }
  
  /**
    Gets the Receiver Signal Strength Indicator (RSSI).
    \return RSSI value (0 to 100).
  */
  uint8_t rssi() const { return rssi_; }
  
  // For unit testing: manually feed a byte into the parser
  void feed_byte(uint8_t b) { process_byte(b); }
};

#endif
