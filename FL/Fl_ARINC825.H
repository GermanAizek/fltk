//
// ARINC 825 / CANaerospace Avionics CAN Bus Protocol class for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#ifndef Fl_ARINC825_H
#define Fl_ARINC825_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>
#include <stdint.h>
#include <stddef.h>

class Fl_ARINC825;
typedef void (*Fl_ARINC825_Callback)(Fl_ARINC825*, void*);

/**
  Cross-platform ARINC 825 (CANaerospace) Protocol wrapper for avionics CAN networks.
  
  Decodes standard 29-bit CAN identifiers:
  - LCC (Logical Communication Channel): 3 bits
  - SOU (Source / Node ID): 7 bits
  - DOC (Data Object Code): 14 bits
  - RCI (Redundancy Channel Identifier): 2 bits
  - Server / Client ID: 3 bits
  Supports CAN FD payloads up to 64 bytes.
*/
class FL_EXPORT Fl_ARINC825 : public Fl_Serial_Port {
public:
  struct CanMessage {
    uint32_t can_id;      // 29-bit CAN Identifier
    uint8_t  lcc;         // Logical Communication Channel (0-7)
    uint8_t  src_node_id; // Source Node ID
    uint16_t doc;         // Data Object Code (e.g. Engine RPM, Airspeed, Actuator)
    uint8_t  rci;         // Redundancy Channel ID
    uint8_t  dlc;         // Data length (0-64)
    uint8_t  data[64];    // Payload data bytes
    bool     is_extended; // 29-bit vs 11-bit
    bool     is_fd;       // CAN FD frame
  };

private:
  uint8_t buffer_[128];
  size_t buf_idx_;
  CanMessage last_msg_;

  Fl_ARINC825_Callback arinc825_cb_;
  void* user_data_;

  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);

public:
  Fl_ARINC825();
  virtual ~Fl_ARINC825();

  int open(const char* port_name);
  void arinc825_callback(Fl_ARINC825_Callback cb, void* user_data = nullptr);

  const CanMessage& last_message() const { return last_msg_; }

  void feed_raw_frame(uint32_t can_id, const uint8_t* payload, uint8_t len, bool is_fd = false);
};

#endif
