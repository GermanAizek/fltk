//
// NATO STANAG 4586 UAV Command & Control Telemetry Protocol class for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#ifndef Fl_STANAG4586_H
#define Fl_STANAG4586_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>
#include <stdint.h>
#include <stddef.h>

class Fl_STANAG4586;
typedef void (*Fl_STANAG4586_Callback)(Fl_STANAG4586*, void*);

/**
  Cross-platform NATO STANAG 4586 Standard (UCS to AV/Payload) Protocol Wrapper.
  
  Decodes standard DLI (Data Link Interface) messages:
  - Sync Word: 0x4586
  - Message ID: 16 bits (e.g. #100 Inertial State, #2000 Vehicle Steering, #3000 Payload Telemetry)
  - Vehicle ID & Subsystem ID
  - Message Sequence Number
  - Data payload & 16-bit CRC
*/
class FL_EXPORT Fl_STANAG4586 : public Fl_Serial_Port {
public:
  struct DliMessage {
    uint32_t vehicle_id;      // Unique Vehicle ID
    uint16_t sync;            // 0x4586
    uint16_t message_id;      // STANAG 4586 Message ID
    uint16_t payload_len;
    uint8_t  subsystem_id;    // Subsystem (0: Airframe, 1: EO/IR, 2: SAR)
    uint8_t  sequence_num;
    bool     is_valid;
    uint8_t  payload[1024];
  };

private:
  DliMessage last_msg_;
  Fl_STANAG4586_Callback stanag_cb_;
  void* user_data_;
  size_t buf_idx_;
  uint8_t buffer_[1024];

  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);

public:
  Fl_STANAG4586();
  virtual ~Fl_STANAG4586();

  int open(const char* port_name);
  void stanag4586_callback(Fl_STANAG4586_Callback cb, void* user_data = nullptr);

  const DliMessage& last_message() const { return last_msg_; }

  void feed_message(uint16_t msg_id, uint32_t v_id, uint8_t subsys, const uint8_t* payload, uint16_t len);
};

#endif
