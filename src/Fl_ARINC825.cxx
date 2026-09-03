//
// ARINC 825 / CANaerospace Avionics CAN Bus Protocol implementation for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#include <FL/Fl_ARINC825.H>
#include <string.h>

Fl_ARINC825::Fl_ARINC825()
  : arinc825_cb_(nullptr), user_data_(nullptr), buf_idx_(0), buffer_(nullptr) {
  memset(&last_msg_, 0, sizeof(last_msg_));
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_ARINC825::~Fl_ARINC825() {
  delete[] buffer_;
}

int Fl_ARINC825::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) return -1;
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  return 0;
}

void Fl_ARINC825::arinc825_callback(Fl_ARINC825_Callback cb, void* user_data) {
  arinc825_cb_ = cb;
  user_data_ = user_data;
}

void Fl_ARINC825::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_ARINC825* self = (Fl_ARINC825*)data;
  uint8_t buf[64];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      self->process_byte(buf[i]);
    }
  }
}

void Fl_ARINC825::process_byte(uint8_t b) {
  // SLCAN / binary CAN framing (start byte 0xAA)
  if (buf_idx_ == 0 && b != 0xAA) return;
  if (!buffer_) buffer_ = new uint8_t[128];
  buffer_[buf_idx_++] = b;

  if (buf_idx_ >= 6) {
    uint8_t dlc = buffer_[5];
    if (dlc > 64) { buf_idx_ = 0; return; }
    if (buf_idx_ == (size_t)(6 + dlc)) {
      uint32_t cid = (buffer_[1] << 24) | (buffer_[2] << 16) | (buffer_[3] << 8) | buffer_[4];
      feed_raw_frame(cid, buffer_ + 6, dlc);
      buf_idx_ = 0;
    }
  }
}

void Fl_ARINC825::feed_raw_frame(uint32_t can_id, const uint8_t* payload, uint8_t len, bool is_fd) {
  last_msg_.can_id = can_id;
  last_msg_.is_extended = (can_id > 0x7FF);
  last_msg_.is_fd = is_fd;

  // ARINC 825 29-bit CAN ID bit breakdown:
  // Bits 28-26: LCC (3 bits)
  // Bits 25-19: Source/Node ID (7 bits)
  // Bits 18-5:  DOC (14 bits)
  // Bits 4-3:   RCI (2 bits)
  // Bits 2-0:   Server ID / Service (3 bits)
  last_msg_.lcc = (can_id >> 26) & 0x07;
  last_msg_.src_node_id = (can_id >> 19) & 0x7F;
  last_msg_.doc = (can_id >> 5) & 0x3FFF;
  last_msg_.rci = (can_id >> 3) & 0x03;

  last_msg_.dlc = (len > 64) ? 64 : len;
  if (payload && last_msg_.dlc > 0) {
    memcpy(last_msg_.data, payload, last_msg_.dlc);
  }

  if (arinc825_cb_) {
    arinc825_cb_(this, user_data_);
  }
}
