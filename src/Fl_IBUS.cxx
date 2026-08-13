//
// IBUS Protocol class for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     https://www.fltk.org/COPYING.php
//
// Please see the following page on how to report bugs and issues:
//
//     https://www.fltk.org/bugs.php
//

#include <FL/Fl_IBUS.H>
#include <string.h>

Fl_IBUS::Fl_IBUS() : buf_idx_(0), ibus_cb_(nullptr), ibus_user_data_(nullptr) {
  memset(channels_, 0, sizeof(channels_));
  memset(buffer_, 0, sizeof(buffer_));
  
  // Register the internal serial callback
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_IBUS::~Fl_IBUS() {
}

int Fl_IBUS::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) {
    return -1;
  }
  
  // Configure for IBUS: 115200 baud, 8N1
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  
  return 0;
}

void Fl_IBUS::ibus_callback(Fl_IBUS_Callback cb, void* user_data) {
  ibus_cb_ = cb;
  ibus_user_data_ = user_data;
}

uint16_t Fl_IBUS::channel(int ch) const {
  if (ch >= 1 && ch <= 14) {
    return channels_[ch - 1];
  }
  return 0;
}

void Fl_IBUS::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_IBUS* ibus = (Fl_IBUS*)data;
  uint8_t buf[128];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      ibus->process_byte(buf[i]);
    }
  }
}

void Fl_IBUS::process_byte(uint8_t b) {
  if (buf_idx_ == 0) {
    // Sync byte 1: 0x20 (Length of the packet)
    if (b == 0x20) {
      buffer_[buf_idx_++] = b;
    }
  } else if (buf_idx_ == 1) {
    // Sync byte 2: 0x40 (Command for RC channels)
    if (b == 0x40) {
      buffer_[buf_idx_++] = b;
    } else {
      buf_idx_ = 0; // Invalid packet type, reset
    }
  } else {
    buffer_[buf_idx_++] = b;
    if (buf_idx_ == 32) {
      // Full 32-byte packet received, check checksum
      uint16_t checksum = 0xFFFF;
      for (int i = 0; i < 30; i++) {
        checksum -= buffer_[i];
      }
      
      uint16_t received_checksum = buffer_[30] | (buffer_[31] << 8);
      
      if (checksum == received_checksum) {
        decode_frame();
        if (ibus_cb_) {
          ibus_cb_(this, ibus_user_data_);
        }
      }
      
      // Reset for next frame
      buf_idx_ = 0;
    }
  }
}

void Fl_IBUS::decode_frame() {
  // Decode 14 channels (16-bit little-endian) from bytes 2 to 29
  for (int i = 0; i < 14; i++) {
    channels_[i] = buffer_[2 + i * 2] | (buffer_[3 + i * 2] << 8);
  }
}
