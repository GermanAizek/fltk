//
// SpaceWire (ECSS-E-ST-50-52C) Spacecraft Telemetry Protocol implementation for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#include <FL/Fl_SpaceWire.H>
#include <string.h>

Fl_SpaceWire::Fl_SpaceWire()
  : buf_idx_(0), spacewire_cb_(nullptr), user_data_(nullptr) {
  memset(&last_packet_, 0, sizeof(last_packet_));
  memset(buffer_, 0, sizeof(buffer_));
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_SpaceWire::~Fl_SpaceWire() {
}

int Fl_SpaceWire::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) return -1;
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  return 0;
}

void Fl_SpaceWire::spacewire_callback(Fl_SpaceWire_Callback cb, void* user_data) {
  spacewire_cb_ = cb;
  user_data_ = user_data;
}

void Fl_SpaceWire::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_SpaceWire* self = (Fl_SpaceWire*)data;
  uint8_t buf[64];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      self->process_byte(buf[i]);
    }
  }
}

void Fl_SpaceWire::process_byte(uint8_t b) {
  // Delimiter EOP (0x00) or EEP (0xFF) over stream
  if (b == 0x00 || b == 0xFF) {
    if (buf_idx_ >= 2) {
      feed_raw_packet(buffer_[0], buffer_[1], buffer_ + 2, buf_idx_ - 2, (b == 0xFF));
    }
    buf_idx_ = 0;
  } else {
    if (buf_idx_ < sizeof(buffer_)) {
      buffer_[buf_idx_++] = b;
    }
  }
}

void Fl_SpaceWire::feed_raw_packet(uint8_t logical_addr, uint8_t proto_id, const uint8_t* data, size_t len, bool eep) {
  last_packet_.logical_address = logical_addr;
  last_packet_.protocol_id = proto_id;
  last_packet_.is_eep = eep;
  last_packet_.is_valid = !eep;

  last_packet_.length = (len > sizeof(last_packet_.payload)) ? sizeof(last_packet_.payload) : len;
  if (data && last_packet_.length > 0) {
    memcpy(last_packet_.payload, data, last_packet_.length);
  }

  if (spacewire_cb_) {
    spacewire_cb_(this, user_data_);
  }
}
