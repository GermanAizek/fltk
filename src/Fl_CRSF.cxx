//
// CRSF Protocol class for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_CRSF.H>
#include <string.h>

Fl_CRSF::Fl_CRSF() : crsf_cb_(nullptr), crsf_user_data_(nullptr), state_(WAIT_SYNC), current_len_(0), buf_idx_(0) {
  memset(channels_, 0, sizeof(channels_));
  memset(buffer_, 0, sizeof(buffer_));
  
  // Register the internal serial callback
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_CRSF::~Fl_CRSF() {
}

int Fl_CRSF::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) {
    return -1;
  }
  
  // Configure for CRSF: 420k baud, 8N1
  if (set_baud_rate(420000) != 0) {
    // Similarly to SBUS, this might fail on strict POSIX interfaces without custom baud rates
  }
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  
  return 0;
}

void Fl_CRSF::crsf_callback(Fl_CRSF_Callback cb, void* user_data) {
  crsf_cb_ = cb;
  crsf_user_data_ = user_data;
}

uint16_t Fl_CRSF::channel(int ch) const {
  if (ch >= 1 && ch <= 16) {
    return channels_[ch - 1];
  }
  return 0;
}

void Fl_CRSF::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_CRSF* crsf = (Fl_CRSF*)data;
  uint8_t buf[128];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      crsf->process_byte(buf[i]);
    }
  }
}

// CRC8 polynomial 0xD5
uint8_t Fl_CRSF::calc_crc8(const uint8_t* data, int len) {
  uint8_t crc = 0;
  for (int i = 0; i < len; i++) {
    crc ^= data[i];
    for (int j = 0; j < 8; j++) {
      if ((crc & 0x80) != 0) {
        crc = (uint8_t)((crc << 1) ^ 0xD5);
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

void Fl_CRSF::process_byte(uint8_t b) {
  switch (state_) {
    case WAIT_SYNC:
      // Valid sync addresses: 0xC8 (Flight Controller), 0xEA (Radio), 0xEE (Receiver)
      if (b == 0xC8 || b == 0xEA || b == 0xEE) {
        buffer_[0] = b;
        state_ = WAIT_LENGTH;
      }
      break;
      
    case WAIT_LENGTH:
      if (b > 62 || b < 2) {
        // Invalid length for CRSF
        state_ = WAIT_SYNC;
      } else {
        buffer_[1] = b;
        current_len_ = b;
        buf_idx_ = 2;
        state_ = WAIT_PAYLOAD;
      }
      break;
      
    case WAIT_PAYLOAD:
      buffer_[buf_idx_++] = b;
      if (buf_idx_ >= current_len_ + 2) {
        // Full packet received
        // Check CRC: CRC covers type + payload (bytes from index 2 to len+1)
        uint8_t expected_crc = buffer_[current_len_ + 1];
        uint8_t actual_crc = calc_crc8(&buffer_[2], current_len_ - 1);
        
        if (actual_crc == expected_crc) {
          uint8_t type = buffer_[2];
          if (type == 0x16) { // RC Channels
            decode_rc_channels();
          }
          if (crsf_cb_) {
            crsf_cb_(this, crsf_user_data_);
          }
        }
        state_ = WAIT_SYNC;
      }
      break;
  }
}

void Fl_CRSF::decode_rc_channels() {
  // Payload for 0x16 starts at buffer_[3], length is 22 bytes
  // Data format is 11 bits per channel, little-endian bit packing
  uint32_t bits = 0;
  uint8_t bits_available = 0;
  int byte_idx = 3;
  
  for (int i = 0; i < 16; i++) {
    while (bits_available < 11) {
      bits |= ((uint32_t)buffer_[byte_idx++]) << bits_available;
      bits_available += 8;
    }
    channels_[i] = bits & 0x07FF;
    bits >>= 11;
    bits_available -= 11;
  }
}
