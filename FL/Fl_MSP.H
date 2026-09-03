//
// MSP Protocol class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_MSP_H
#define Fl_MSP_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>

class Fl_MSP;

/**
  Callback type for Fl_MSP.
  \param msp The Fl_MSP instance.
  \param cmd The parsed MSP Command ID.
  \param payload Pointer to the parsed payload data.
  \param size The size of the payload in bytes.
  \param user_data User-defined data pointer.
*/
typedef void (*Fl_MSP_Callback)(Fl_MSP* msp, uint8_t cmd, const uint8_t* payload, uint8_t size, void* user_data);

/**
  Cross-platform MSP (MultiWii Serial Protocol) wrapper.
  
  Inherits from Fl_Serial_Port to provide decoding of MSP V1 frames.
  The protocol typically operates at 115,200 baud, 8 data bits, no parity, 
  and 1 stop bit (8N1).
  
  This class maintains a state machine to parse variable-length frames and
  validates the XOR checksum. A callback is triggered when a valid frame
  is successfully decoded.
*/
class FL_EXPORT Fl_MSP : public Fl_Serial_Port {
private:
  enum State : uint8_t {
    WAIT_HEADER_1 = 0, // '$'
    WAIT_HEADER_2,     // 'M'
    WAIT_DIRECTION,    // '<' or '>'
    WAIT_SIZE,
    WAIT_CMD,
    WAIT_PAYLOAD,
    WAIT_CHECKSUM
  };

  Fl_MSP_Callback msp_cb_;
  void* msp_user_data_;
  uint8_t* payload_buffer_;
  State state_;
  uint8_t payload_size_;
  uint8_t cmd_;
  uint8_t current_checksum_;
  uint8_t buf_idx_;

  // Internal serial callback to feed the parser
  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);

public:
  Fl_MSP();
  virtual ~Fl_MSP();

  /**
    Opens the serial port and automatically configures it for MSP
    (115,200 baud, 8 data bits, NO parity, 1 stop bit).
    \param port_name The path or name of the port.
    \return 0 on success, or -1 on error.
  */
  int open(const char* port_name);

  /**
    Registers a callback function to be called when a valid MSP frame is received.
    \param cb The callback function.
    \param user_data Optional user data.
  */
  void msp_callback(Fl_MSP_Callback cb, void* user_data = nullptr);

  /**
    Sends an MSP V1 command to the flight controller.
    Constructs the frame `$M<` + size + cmd + payload + checksum.
    \param cmd The MSP Command ID.
    \param payload Pointer to the payload data.
    \param size The size of the payload.
  */
  void send_command(uint8_t cmd, const uint8_t* payload, uint8_t size);
  
  // For unit testing: manually feed a byte into the parser
  void feed_byte(uint8_t b) { process_byte(b); }
};

#endif
