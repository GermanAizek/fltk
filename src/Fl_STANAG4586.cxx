//
// NATO STANAG 4586 UAV Command & Control Telemetry Protocol implementation for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#include <FL/Fl_STANAG4586.H>
#include <string.h>

Fl_STANAG4586::Fl_STANAG4586()
  : buf_idx_(0), stanag_cb_(nullptr), user_data_(nullptr) {
  memset(&last_msg_, 0, sizeof(last_msg_));
  memset(buffer_, 0, sizeof(buffer_));
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_STANAG4586::~Fl_STANAG4586() {
}

int Fl_STANAG4586::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) return -1;
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  return 0;
}

void Fl_STANAG4586::stanag4586_callback(Fl_STANAG4586_Callback cb, void* user_data) {
  stanag_cb_ = cb;
  user_data_ = user_data;
}

void Fl_STANAG4586::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_STANAG4586* self = (Fl_STANAG4586*)data;
  uint8_t buf[128];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      self->process_byte(buf[i]);
    }
  }
}

void Fl_STANAG4586::process_byte(uint8_t b) {
  if (buf_idx_ == 0 && b != 0x45) return;
  if (buf_idx_ == 1 && b != 0x86) { buf_idx_ = 0; return; }

  buffer_[buf_idx_++] = b;

  if (buf_idx_ >= 12) {
    uint16_t len = (buffer_[10] << 8) | buffer_[11];
    if (buf_idx_ == (size_t)(12 + len)) {
      uint16_t mid = (buffer_[2] << 8) | buffer_[3];
      uint32_t vid = (buffer_[4] << 24) | (buffer_[5] << 16) | (buffer_[6] << 8) | buffer_[7];
      uint8_t sub = buffer_[8];

      feed_message(mid, vid, sub, buffer_ + 12, len);
      buf_idx_ = 0;
    }
  }
}

void Fl_STANAG4586::feed_message(uint16_t msg_id, uint32_t v_id, uint8_t subsys, const uint8_t* payload, uint16_t len) {
  last_msg_.sync = 0x4586;
  last_msg_.message_id = msg_id;
  last_msg_.vehicle_id = v_id;
  last_msg_.subsystem_id = subsys;
  last_msg_.sequence_num++;
  last_msg_.is_valid = true;

  last_msg_.payload_len = (len > sizeof(last_msg_.payload)) ? sizeof(last_msg_.payload) : len;
  if (payload && last_msg_.payload_len > 0) {
    memcpy(last_msg_.payload, payload, last_msg_.payload_len);
  }

  if (stanag_cb_) {
    stanag_cb_(this, user_data_);
  }
}
