//
// MAVLink 2.0 Micro Air Vehicle Telemetry Protocol implementation for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#include <FL/Fl_MAVLink2.H>
#include <string.h>

Fl_MAVLink2::Fl_MAVLink2()
  : mav_cb_(nullptr), user_data_(nullptr), buf_idx_(0) {
  memset(&last_msg_, 0, sizeof(last_msg_));
  memset(buffer_, 0, sizeof(buffer_));
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_MAVLink2::~Fl_MAVLink2() {
}

int Fl_MAVLink2::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) return -1;
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  return 0;
}

void Fl_MAVLink2::mavlink_callback(Fl_MAVLink2_Callback cb, void* user_data) {
  mav_cb_ = cb;
  user_data_ = user_data;
}

void Fl_MAVLink2::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_MAVLink2* self = (Fl_MAVLink2*)data;
  uint8_t buf[128];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      self->process_byte(buf[i]);
    }
  }
}

uint16_t Fl_MAVLink2::crc_accumulate(uint8_t b, uint16_t crc) const {
  uint8_t ch = b ^ (uint8_t)(crc & 0x00FF);
  ch = ch ^ (ch << 4);
  return (crc >> 8) ^ ((uint16_t)ch << 8) ^ ((uint16_t)ch << 3) ^ ((uint16_t)ch >> 4);
}

void Fl_MAVLink2::process_byte(uint8_t b) {
  if (buf_idx_ == 0 && b != 0xFD) return; // MAVLink 2 STX
  buffer_[buf_idx_++] = b;

  if (buf_idx_ >= 10) {
    uint8_t payload_len = buffer_[1];
    size_t total_len = 10 + payload_len + 2; // header (10) + payload + crc (2)

    if (buf_idx_ == total_len) {
      last_msg_.payload_len = payload_len;
      last_msg_.incompat_flags = buffer_[2];
      last_msg_.compat_flags = buffer_[3];
      last_msg_.seq = buffer_[4];
      last_msg_.sys_id = buffer_[5];
      last_msg_.comp_id = buffer_[6];
      last_msg_.msg_id = buffer_[7] | (buffer_[8] << 8) | (buffer_[9] << 16);
      
      memcpy(last_msg_.payload, buffer_ + 10, payload_len);
      last_msg_.checksum = buffer_[10 + payload_len] | (buffer_[10 + payload_len + 1] << 8);
      last_msg_.crc_valid = true;

      if (mav_cb_) mav_cb_(this, user_data_);
      buf_idx_ = 0;
    }
  }
}

void Fl_MAVLink2::feed_message(uint8_t sys_id, uint8_t comp_id, uint32_t msg_id, const uint8_t* payload, uint8_t len) {
  last_msg_.sys_id = sys_id;
  last_msg_.comp_id = comp_id;
  last_msg_.msg_id = msg_id;
  last_msg_.payload_len = len;
  last_msg_.seq++;
  last_msg_.crc_valid = true;

  if (payload && len > 0) {
    memcpy(last_msg_.payload, payload, len);
  }

  if (mav_cb_) {
    mav_cb_(this, user_data_);
  }
}
