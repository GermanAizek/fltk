//
// SpaceWire (ECSS-E-ST-50-52C) Spacecraft Telemetry Protocol class for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#ifndef Fl_SpaceWire_H
#define Fl_SpaceWire_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>
#include <stdint.h>
#include <stddef.h>

class Fl_SpaceWire;
typedef void (*Fl_SpaceWire_Callback)(Fl_SpaceWire*, void*);

/**
  Cross-platform SpaceWire (ECSS-E-ST-50-52C) Protocol wrapper for space avionics.
  
  Decodes space packets with:
  - Logical Address (0xFE for RMAP, 0x01-0xFD node routing)
  - Protocol ID (0x01 RMAP, 0x02 CCSDS, 0x03 GOES-N, 0xF0 Custom)
  - EOP (End of Packet) / EEP (Error End of Packet) delimiters
  - Payload buffer and character parity check
*/
class FL_EXPORT Fl_SpaceWire : public Fl_Serial_Port {
public:
  struct Packet {
    uint16_t length;          // Payload byte count
    uint8_t  logical_address; // SpaceWire Logical Address
    uint8_t  protocol_id;     // Protocol Identifier
    bool     is_eep;          // Error End of Packet flag
    bool     is_valid;
    uint8_t  payload[1024];   // Data bytes
  };

private:
  Packet last_packet_;
  Fl_SpaceWire_Callback spacewire_cb_;
  void* user_data_;
  size_t buf_idx_;
  uint8_t buffer_[1024];

  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);

public:
  Fl_SpaceWire();
  virtual ~Fl_SpaceWire();

  int open(const char* port_name);
  void spacewire_callback(Fl_SpaceWire_Callback cb, void* user_data = nullptr);

  const Packet& last_packet() const { return last_packet_; }

  void feed_raw_packet(uint8_t logical_addr, uint8_t proto_id, const uint8_t* data, size_t len, bool eep = false);
};

#endif
