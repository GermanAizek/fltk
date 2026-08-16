//
// SUMD Protocol class for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_SUMD.H>
#include <string.h>

Fl_SUMD::Fl_SUMD() : sumd_cb_(nullptr), sumd_user_data_(nullptr),
                     state_(WAIT_HEADER), num_channels_(0), status_(0), 
                     buf_idx_(0), current_len_(0) {
  memset(channels_, 0, sizeof(channels_));
  memset(buffer_, 0, sizeof(buffer_));
  
  // Register the internal serial callback
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_SUMD::~Fl_SUMD() {
}

int Fl_SUMD::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) {
    return -1;
  }
  
  // Configure for SUMD: 115200 baud, 8N1
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  
  return 0;
}

void Fl_SUMD::sumd_callback(Fl_SUMD_Callback cb, void* user_data) {
  sumd_cb_ = cb;
  sumd_user_data_ = user_data;
}

uint16_t Fl_SUMD::channel(int ch) const {
  if (ch >= 1 && ch <= num_channels_ && ch <= 32) {
    return channels_[ch - 1];
  }
  return 0;
}

void Fl_SUMD::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_SUMD* sumd = (Fl_SUMD*)data;
  uint8_t buf[256];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      sumd->process_byte(buf[i]);
    }
  }
}

// CRC16-CCITT implementation for Graupner SUMD (same poly as XBUS)
uint16_t Fl_SUMD::calc_crc16(const uint8_t* data, int len) {
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

void Fl_SUMD::process_byte(uint8_t b) {
  switch (state_) {
    case WAIT_HEADER:
      if (b == 0xA8) { // SUMD Header
        buffer_[0] = b;
        buf_idx_ = 1;
        state_ = WAIT_STATUS;
      }
      break;
      
    case WAIT_STATUS:
      buffer_[buf_idx_++] = b;
      state_ = WAIT_COUNT;
      break;
      
    case WAIT_COUNT:
      if (b > 32 || b == 0) { // Invalid channel count
        state_ = WAIT_HEADER;
      } else {
        buffer_[buf_idx_++] = b;
        num_channels_ = b;
        // Length of frame = 1(header) + 1(status) + 1(count) + N*2(payload) + 2(CRC)
        current_len_ = 3 + (num_channels_ * 2) + 2;
        state_ = WAIT_PAYLOAD;
      }
      break;
      
    case WAIT_PAYLOAD:
      buffer_[buf_idx_++] = b;
      if (buf_idx_ >= current_len_) {
        // Full packet received, check CRC
        // CRC is calculated over the frame excluding the last 2 bytes
        uint16_t crc = calc_crc16(buffer_, current_len_ - 2);
        
        // SUMD sends CRC in big-endian format
        uint16_t received_crc = (buffer_[current_len_ - 2] << 8) | buffer_[current_len_ - 1];
        
        if (crc == received_crc) {
          status_ = buffer_[1];
          decode_frame();
          if (sumd_cb_) {
            sumd_cb_(this, sumd_user_data_);
          }
        }
        
        // Reset for next frame
        state_ = WAIT_HEADER;
      }
      break;
  }
}

void Fl_SUMD::decode_frame() {
  // Decode N channels (16-bit big-endian) starting from byte 3
  for (int i = 0; i < num_channels_; i++) {
    channels_[i] = (buffer_[3 + i * 2] << 8) | buffer_[4 + i * 2];
  }
}
