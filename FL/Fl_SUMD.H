//
// SUMD Protocol class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_SUMD_H
#define Fl_SUMD_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>

class Fl_SUMD;
typedef void (*Fl_SUMD_Callback)(Fl_SUMD*, void*);

/**
  Cross-platform Graupner SUMD (Digital SUM) protocol wrapper.
  
  Inherits from Fl_Serial_Port to provide decoding of SUMD RC frames.
  The protocol operates at 115,200 baud, 8 data bits, no parity, 
  and 1 stop bit (8N1).
  
  This class buffers bytes to parse variable-length frames based on the
  transmitted channel count and validates the 16-bit CRC checksum. 
  A callback is triggered when a valid frame is successfully decoded.
*/
class FL_EXPORT Fl_SUMD : public Fl_Serial_Port {
private:
  enum State : uint8_t {
    WAIT_HEADER = 0,
    WAIT_STATUS,
    WAIT_COUNT,
    WAIT_PAYLOAD
  };

  Fl_SUMD_Callback sumd_cb_;
  void* sumd_user_data_;
  uint16_t channels_[32]; // Max 32 channels (16-bit)
  uint8_t buffer_[128];   // Max SUMD frame is roughly 3 + 32*2 + 2 = 69 bytes
  State state_;
  uint8_t num_channels_;
  uint8_t status_; // 0x01 = valid, 0x00 = failsafe, 0x81 = failsafe valid
  uint8_t buf_idx_;
  uint8_t current_len_;

  // Internal serial callback to feed the parser
  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);
  void decode_frame();
  uint16_t calc_crc16(const uint8_t* data, int len);

public:
  Fl_SUMD();
  virtual ~Fl_SUMD();

  /**
    Opens the serial port and automatically configures it for SUMD
    (115,200 baud, 8 data bits, NO parity, 1 stop bit).
    \param port_name The path or name of the port.
    \return 0 on success, or -1 on error.
  */
  int open(const char* port_name);

  /**
    Registers a callback function to be called when a valid SUMD frame is received.
    \param cb The callback function.
    \param user_data Optional user data.
  */
  void sumd_callback(Fl_SUMD_Callback cb, void* user_data = nullptr);

  /**
    Gets the value of a specific channel.
    \param ch The channel number (1 to 32).
    \return The 16-bit value for the channel (typically around 4096).
  */
  uint16_t channel(int ch) const;
  
  /**
    Gets the number of channels received in the last valid frame.
    \return The number of channels (typically 8, 12, or 16).
  */
  uint8_t channel_count() const { return num_channels_; }
  
  /**
    Gets the status byte of the last valid frame.
    0x01 usually means valid.
    0x00 usually means failsafe / no signal.
    0x81 usually means valid signal but failsafe bit set.
    \return The status byte.
  */
  uint8_t status() const { return status_; }
  
  // For unit testing: manually feed a byte into the parser
  void feed_byte(uint8_t b) { process_byte(b); }
};

#endif
