//
// MAVLink 2.0 Micro Air Vehicle Telemetry Protocol class for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#ifndef Fl_MAVLink2_H
#define Fl_MAVLink2_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>
#include <stdint.h>
#include <stddef.h>
#include <vector>

class Fl_MAVLink2;
typedef void (*Fl_MAVLink2_Callback)(Fl_MAVLink2*, void*);

/**
  Cross-platform MAVLink 2.0 UAV / Drone Telemetry Protocol Parser.
  
  Decodes standard MAVLink 2 frame packets:
  - Magic STX byte: 0xFD
  - Payload length: 8 bits
  - Incompatibility & Compatibility flags: 16 bits
  - Packet sequence: 8 bits
  - System ID & Component ID: 8 bits each
  - Message ID: 24 bits (e.g. #0 HEARTBEAT, #30 ATTITUDE, #33 GLOBAL_POSITION_INT, #74 VFR_HUD)
  - Payload buffer
  - Checksum: 16-bit CRC-MCRF4XX
*/
class FL_EXPORT Fl_MAVLink2 : public Fl_Serial_Port {
public:
  struct Message {
    uint32_t msg_id;          // 24-bit Message ID
    uint16_t checksum;
    uint8_t  payload_len;     // 0 to 255
    uint8_t  incompat_flags;
    uint8_t  compat_flags;
    uint8_t  seq;             // Packet sequence
    uint8_t  sys_id;          // System ID (e.g. 1)
    uint8_t  comp_id;         // Component ID (e.g. 1 Autopilot)
    bool     crc_valid;
    std::vector<uint8_t> payload;
  };

private:
  Message last_msg_;
  Fl_MAVLink2_Callback mav_cb_;
  void* user_data_;
  std::vector<uint8_t> buffer_;

  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);
  static uint16_t crc_accumulate(uint8_t b, uint16_t crc);

public:
  Fl_MAVLink2();
  virtual ~Fl_MAVLink2();

  int open(const char* port_name);
  void mavlink_callback(Fl_MAVLink2_Callback cb, void* user_data = nullptr);

  const Message& last_message() const { return last_msg_; }

  void feed_message(uint8_t sys_id, uint8_t comp_id, uint32_t msg_id, const uint8_t* payload, uint8_t len);
};

#endif
