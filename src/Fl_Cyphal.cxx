//
// OpenCyphal / UAVCAN v1.0 Distributed Avionics Protocol implementation for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#include <FL/Fl_Cyphal.H>
#include <string.h>

Fl_Cyphal::Fl_Cyphal()
  : cyphal_cb_(nullptr), user_data_(nullptr) {
  last_transfer_.port_id = 0;
  last_transfer_.payload_len = 0;
  last_transfer_.priority = 0;
  last_transfer_.src_node_id = 0;
  last_transfer_.dst_node_id = 0;
  last_transfer_.transfer_id = 0;
  last_transfer_.is_service = false;
  last_transfer_.start_of_transfer = false;
  last_transfer_.end_of_transfer = false;
  last_transfer_.is_valid = false;
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_Cyphal::~Fl_Cyphal() {
}

int Fl_Cyphal::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) return -1;
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  return 0;
}

void Fl_Cyphal::cyphal_callback(Fl_Cyphal_Callback cb, void* user_data) {
  cyphal_cb_ = cb;
  user_data_ = user_data;
}

void Fl_Cyphal::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_Cyphal* self = (Fl_Cyphal*)data;
  uint8_t buf[128];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      self->process_byte(buf[i]);
    }
  }
}

void Fl_Cyphal::process_byte(uint8_t b) {
  if (buffer_.empty() && b != 0xC0) return; // Cyphal serial delimiter
  buffer_.push_back(b);

  if (buffer_.size() >= 8) {
    uint16_t len = (buffer_[1] << 8) | buffer_[2];
    if (buffer_.size() == (size_t)(8 + len)) {
      feed_transfer(buffer_[3], (buffer_[4] << 8) | buffer_[5], buffer_[6] != 0, buffer_[7], 255, 0, buffer_.data() + 8, len);
      buffer_.clear();
    }
  }
}

void Fl_Cyphal::feed_transfer(uint8_t priority, uint16_t port_id, bool is_service, uint8_t src_node, uint8_t dst_node, uint8_t transfer_id, const uint8_t* payload, uint16_t len) {
  last_transfer_.priority = priority;
  last_transfer_.port_id = port_id;
  last_transfer_.is_service = is_service;
  last_transfer_.src_node_id = src_node;
  last_transfer_.dst_node_id = dst_node;
  last_transfer_.transfer_id = transfer_id;
  last_transfer_.start_of_transfer = true;
  last_transfer_.end_of_transfer = true;
  last_transfer_.is_valid = true;
  last_transfer_.payload_len = len;

  if (payload && len > 0) {
    last_transfer_.payload.assign(payload, payload + len);
  } else {
    last_transfer_.payload.clear();
  }

  if (cyphal_cb_) {
    cyphal_cb_(this, user_data_);
  }
}
