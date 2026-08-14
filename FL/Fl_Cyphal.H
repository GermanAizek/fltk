//
// OpenCyphal / UAVCAN v1.0 Distributed Avionics Protocol class for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#ifndef Fl_Cyphal_H
#define Fl_Cyphal_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>
#include <stdint.h>
#include <stddef.h>

class Fl_Cyphal;
typedef void (*Fl_Cyphal_Callback)(Fl_Cyphal*, void*);

/**
  Cross-platform OpenCyphal (UAVCAN v1.0) Real-Time Pub/Sub Protocol Wrapper.
  
  Decodes Cyphal CAN / UDP transport frames:
  - Priority: 3 bits (Exceptional, Immediate, Fast, High, Nominal, Low, Slow, Optional)
  - Subject ID (Message Broadcast) or Service ID (Request/Response)
  - Source Node ID & Destination Node ID
  - Transfer ID (5-bit cyclic transfer counter)
  - Tail byte: Start of Transfer (SOT), End of Transfer (EOT), Toggle bit
*/
class FL_EXPORT Fl_Cyphal : public Fl_Serial_Port {
public:
  struct Transfer {
    uint8_t  priority;        // 0 to 7
    uint16_t port_id;         // Subject ID (0-8191) or Service ID
    bool     is_service;      // Message vs Service
    uint8_t  src_node_id;     // 0 to 127
    uint8_t  dst_node_id;     // Destination Node (or 255 for broadcast)
    uint8_t  transfer_id;     // 0 to 31
    bool     start_of_transfer;
    bool     end_of_transfer;
    uint16_t payload_len;
    uint8_t  payload[512];
    bool     is_valid;
  };

private:
  uint8_t buffer_[512];
  size_t buf_idx_;
  Transfer last_transfer_;

  Fl_Cyphal_Callback cyphal_cb_;
  void* user_data_;

  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);

public:
  Fl_Cyphal();
  virtual ~Fl_Cyphal();

  int open(const char* port_name);
  void cyphal_callback(Fl_Cyphal_Callback cb, void* user_data = nullptr);

  const Transfer& last_transfer() const { return last_transfer_; }

  void feed_transfer(uint8_t priority, uint16_t port_id, bool is_service, uint8_t src_node, uint8_t dst_node, uint8_t transfer_id, const uint8_t* payload, uint16_t len);
};

#endif
