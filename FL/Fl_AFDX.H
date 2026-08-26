//
// AFDX (ARINC 664) Protocol class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_AFDX_H
#define Fl_AFDX_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>
#include <vector>

class Fl_AFDX;
typedef void (*Fl_AFDX_Callback)(Fl_AFDX*, void*);

/**
  Cross-platform AFDX (ARINC 664) protocol wrapper over serial.
  
  Inherits from Fl_Serial_Port to provide decoding of AFDX frames encapsulated
  over a serial port. Since AFDX frames are Ethernet frames (up to 1518 bytes) 
  and variable length, this class uses SLIP (Serial Line Internet Protocol) 
  encoding (RFC 1055) to delimit frames over the serial stream.
  
  An AFDX frame features a MAC Destination address where the last 2 bytes 
  contain the Virtual Link (VL) ID, and a 1-byte Sequence Number at the end of the UDP payload.
  
  The parser maintains an internal buffer and triggers a user-defined callback
  whenever a complete SLIP frame is successfully decoded and minimally validated as AFDX.
*/
class FL_EXPORT Fl_AFDX : public Fl_Serial_Port {
public:
  /**
    Structure representing a decoded AFDX frame summary.
  */
  struct FrameInfo {
    int payload_length;    // Length of the IP/UDP payload (excluding MAC/SEQ)
    uint16_t virtual_link; // VL ID extracted from Dest MAC
    uint8_t seq_num;       // Sequence Number extracted from the end of the frame
    bool is_valid;         // Basic validation flag
  };

private:
  FrameInfo last_frame_;
  Fl_AFDX_Callback afdx_cb_;
  void* afdx_user_data_;
  bool escape_flag_;
  std::vector<uint8_t> buffer_; // Dynamic AFDX frame buffer

  // Internal serial callback to feed the parser
  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);
  void decode_frame(const uint8_t* frame, int length);

public:
  Fl_AFDX();
  virtual ~Fl_AFDX();

  /**
    Opens the serial port.
    \param port_name The path or name of the port.
    \return 0 on success, or -1 on error.
  */
  int open(const char* port_name);

  /**
    Registers a callback function to be called when a valid AFDX frame is received.
    \param cb The callback function.
    \param user_data Optional user data.
  */
  void afdx_callback(Fl_AFDX_Callback cb, void* user_data = nullptr);

  /**
    Gets the summary of the last received AFDX frame.
    \return The last decoded frame info structure.
  */
  const FrameInfo& last_frame() const { return last_frame_; }
  
  /**
    Gets the raw frame buffer.
    \return Pointer to the internal frame buffer.
  */
  const uint8_t* raw_buffer() const { return buffer_.empty() ? nullptr : buffer_.data(); }
  
  // For unit testing: manually feed a byte into the parser
  void feed_byte(uint8_t b) { process_byte(b); }
  
  // For unit testing: manually feed a complete raw frame
  void feed_raw_frame(const uint8_t* data, int len);
};

#endif
