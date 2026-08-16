//
// SBUS Protocol class for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_SBUS.H>
#include <string.h>

Fl_SBUS::Fl_SBUS() : sbus_cb_(nullptr), sbus_user_data_(nullptr), buf_idx_(0), failsafe_(false), frame_lost_(false) {
  memset(channels_, 0, sizeof(channels_));
  memset(buffer_, 0, sizeof(buffer_));
  
  // Register the internal serial callback
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_SBUS::~Fl_SBUS() {
}

int Fl_SBUS::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) {
    return -1;
  }
  
  // Configure for SBUS: 100k baud, 8 data bits, Even parity, 2 stop bits
  if (set_baud_rate(100000) != 0) {
    // Non-standard baud rate may fail on standard POSIX without custom BOTHER ioctls.
    // We ignore the error here as some drivers might still work or have been pre-configured.
  }
  set_data_bits(DATA_8);
  set_parity(PARITY_EVEN);
  set_stop_bits(STOP_2);
  
  return 0;
}

void Fl_SBUS::sbus_callback(Fl_SBUS_Callback cb, void* user_data) {
  sbus_cb_ = cb;
  sbus_user_data_ = user_data;
}

uint16_t Fl_SBUS::channel(int ch) const {
  if (ch >= 1 && ch <= 18) {
    return channels_[ch - 1];
  }
  return 0;
}

void Fl_SBUS::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_SBUS* sbus = (Fl_SBUS*)data;
  uint8_t buf[64];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      sbus->process_byte(buf[i]);
    }
  }
}

void Fl_SBUS::process_byte(uint8_t b) {
  if (buf_idx_ == 0) {
    if (b == 0x0F) {
      buffer_[buf_idx_++] = b;
    }
  } else {
    buffer_[buf_idx_++] = b;
    if (buf_idx_ == 25) {
      // Complete frame received, check footer
      // SBUS footer is typically 0x00, but can be 0x04, 0x14, 0x24, 0x34 depending on the version
      uint8_t footer = buffer_[24];
      if (footer == 0x00 || footer == 0x04 || footer == 0x14 || footer == 0x24 || footer == 0x34) {
        decode_frame();
        if (sbus_cb_) {
          sbus_cb_(this, sbus_user_data_);
        }
      }
      // Reset for next frame
      buf_idx_ = 0;
    }
  }
}

void Fl_SBUS::decode_frame() {
  // Decode 16 proportional channels (11 bits each) from 22 bytes
  channels_[0]  = ((buffer_[1]    |buffer_[2]<<8)                 & 0x07FF);
  channels_[1]  = ((buffer_[2]>>3 |buffer_[3]<<5)                 & 0x07FF);
  channels_[2]  = ((buffer_[3]>>6 |buffer_[4]<<2 |buffer_[5]<<10) & 0x07FF);
  channels_[3]  = ((buffer_[5]>>1 |buffer_[6]<<7)                 & 0x07FF);
  channels_[4]  = ((buffer_[6]>>4 |buffer_[7]<<4)                 & 0x07FF);
  channels_[5]  = ((buffer_[7]>>7 |buffer_[8]<<1 |buffer_[9]<<9)  & 0x07FF);
  channels_[6]  = ((buffer_[9]>>2 |buffer_[10]<<6)                & 0x07FF);
  channels_[7]  = ((buffer_[10]>>5|buffer_[11]<<3)                & 0x07FF);
  
  channels_[8]  = ((buffer_[12]   |buffer_[13]<<8)                & 0x07FF);
  channels_[9]  = ((buffer_[13]>>3|buffer_[14]<<5)                & 0x07FF);
  channels_[10] = ((buffer_[14]>>6|buffer_[15]<<2|buffer_[16]<<10)& 0x07FF);
  channels_[11] = ((buffer_[16]>>1|buffer_[17]<<7)                & 0x07FF);
  channels_[12] = ((buffer_[17]>>4|buffer_[18]<<4)                & 0x07FF);
  channels_[13] = ((buffer_[18]>>7|buffer_[19]<<1|buffer_[20]<<9) & 0x07FF);
  channels_[14] = ((buffer_[20]>>2|buffer_[21]<<6)                & 0x07FF);
  channels_[15] = ((buffer_[21]>>5|buffer_[22]<<3)                & 0x07FF);
  
  // Decode digital channels and flags
  uint8_t flags = buffer_[23];
  channels_[16] = (flags & 0x01) ? 1 : 0;
  channels_[17] = (flags & 0x02) ? 1 : 0;
  frame_lost_   = (flags & 0x04) != 0;
  failsafe_     = (flags & 0x08) != 0;
}
