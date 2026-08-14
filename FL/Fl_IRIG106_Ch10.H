//
// IRIG 106 Chapter 10 / 11 Flight Test Telemetry Protocol class for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#ifndef Fl_IRIG106_Ch10_H
#define Fl_IRIG106_Ch10_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>
#include <stdint.h>
#include <stddef.h>

class Fl_IRIG106_Ch10;
typedef void (*Fl_IRIG106_Callback)(Fl_IRIG106_Ch10*, void*);

/**
  Cross-platform IRIG 106 Chapter 10 / 11 Flight Test Telemetry Packet Parser.
  
  Parses standardized 24-byte packet headers:
  - Sync Pattern: 0xEB25
  - Channel ID: 16 bits
  - Packet Length: 32 bits
  - Data Type: 8 bits (0x01 Time, 0x19 MIL-STD-1553, 0x38 ARINC 429, 0x40 Video, 0x11 PCM)
  - Sequence Number: 8 bits
  - Relative Time Counter (RTC): 48 bits
  - Header & Data Checksum validation
*/
class FL_EXPORT Fl_IRIG106_Ch10 : public Fl_Serial_Port {
public:
  struct PacketHeader {
    uint16_t sync;            // 0xEB25
    uint16_t channel_id;      // Telemetry Channel
    uint32_t packet_len;      // Total packet length in bytes
    uint32_t data_len;        // Payload data length in bytes
    uint8_t  data_type;       // DataType ID (0x01: Time, 0x19: 1553, 0x38: A429, etc.)
    uint8_t  seq_num;         // Packet sequence counter
    uint8_t  packet_flags;    // Time source & secondary header flags
    uint8_t  data_type_ver;   // Data type version
    uint64_t rtc_timestamp;   // Relative Time Counter (100ns units)
    uint16_t header_checksum; // 16-bit header checksum
    bool     checksum_ok;
  };

private:
  uint8_t buffer_[65536];
  size_t buf_idx_;
  PacketHeader last_header_;
  uint8_t payload_[65536];
  size_t payload_len_;

  Fl_IRIG106_Callback irig_cb_;
  void* user_data_;

  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);

public:
  Fl_IRIG106_Ch10();
  virtual ~Fl_IRIG106_Ch10();

  int open(const char* port_name);
  void irig_callback(Fl_IRIG106_Callback cb, void* user_data = nullptr);

  const PacketHeader& last_header() const { return last_header_; }
  const uint8_t* last_payload() const { return payload_; }
  size_t last_payload_len() const { return payload_len_; }

  void feed_raw_packet(const uint8_t* pkt_data, size_t len);
};

#endif
