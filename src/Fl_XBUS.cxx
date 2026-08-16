//
// XBUS Protocol class for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_XBUS.H>
#include <string.h>

Fl_XBUS::Fl_XBUS() : xbus_cb_(nullptr), xbus_user_data_(nullptr), buf_idx_(0) {
  memset(channels_, 0, sizeof(channels_));
  memset(buffer_, 0, sizeof(buffer_));
  
  // Register the internal serial callback
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_XBUS::~Fl_XBUS() {
}

int Fl_XBUS::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) {
    return -1;
  }
  
  // Configure for XBUS: 115200 baud, 8N1
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  
  return 0;
}

void Fl_XBUS::xbus_callback(Fl_XBUS_Callback cb, void* user_data) {
  xbus_cb_ = cb;
  xbus_user_data_ = user_data;
}

uint16_t Fl_XBUS::channel(int ch) const {
  if (ch >= 1 && ch <= 16) {
    return channels_[ch - 1];
  }
  return 0;
}

void Fl_XBUS::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_XBUS* xbus = (Fl_XBUS*)data;
  uint8_t buf[128];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      xbus->process_byte(buf[i]);
    }
  }
}

// CRC16-CCITT implementation for JR XBus Mode B (SRXL)
uint16_t Fl_XBUS::calc_crc16(const uint8_t* data, int len) {
  uint16_t crc = 0x0000;
  for (int i = 0; i < len; i++) {
    crc ^= (data[i] << 8);
    for (int j = 0; j < 8; j++) {
      if ((crc & 0x8000) > 0) {
        crc = (crc << 1) ^ 0x1021;
      } else {
        crc = (crc << 1);
      }
    }
  }
  return crc;
}

void Fl_XBUS::process_byte(uint8_t b) {
  if (buf_idx_ == 0) {
    // Header byte: 0xA1 (16 channels)
    if (b == 0xA1) {
      buffer_[buf_idx_++] = b;
    }
  } else {
    buffer_[buf_idx_++] = b;
    if (buf_idx_ == 35) {
      // Full 35-byte packet received, check CRC
      uint16_t crc = calc_crc16(buffer_, 33); // CRC covers header and payload
      
      // XBUS/SRXL sends CRC in big-endian format
      uint16_t received_crc = (buffer_[33] << 8) | buffer_[34];
      
      if (crc == received_crc) {
        decode_frame();
        if (xbus_cb_) {
          xbus_cb_(this, xbus_user_data_);
        }
      }
      
      // Reset for next frame
      buf_idx_ = 0;
    }
  }
}

void Fl_XBUS::decode_frame() {
  // Decode 16 channels (16-bit big-endian) from bytes 1 to 32
  for (int i = 0; i < 16; i++) {
    channels_[i] = (buffer_[1 + i * 2] << 8) | buffer_[2 + i * 2];
  }
}
