//
// ARINC 429 Protocol class for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_ARINC429.H>
#include <string.h>

Fl_ARINC429::Fl_ARINC429() : arinc429_cb_(nullptr), arinc429_user_data_(nullptr), buf_idx_(0) {
  memset(buffer_, 0, sizeof(buffer_));
  memset(&last_word_, 0, sizeof(last_word_));
  
  // Register the internal serial callback
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_ARINC429::~Fl_ARINC429() {
}

int Fl_ARINC429::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) {
    return -1;
  }
  
  // Default ARINC 429 serial adapter settings (commonly 8N1 or similar depending on the adapter hardware)
  // The actual ARINC 429 baud rate is 100kbps or 12.5kbps, but serial adapters might operate at 115200 or other speeds.
  // We'll set a reasonable default and expect users to configure it if needed.
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  
  return 0;
}

void Fl_ARINC429::arinc429_callback(Fl_ARINC429_Callback cb, void* user_data) {
  arinc429_cb_ = cb;
  arinc429_user_data_ = user_data;
}

void Fl_ARINC429::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_ARINC429* arinc = (Fl_ARINC429*)data;
  uint8_t buf[64];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      arinc->process_byte(buf[i]);
    }
  }
}

void Fl_ARINC429::process_byte(uint8_t b) {
  buffer_[buf_idx_++] = b;
  if (buf_idx_ == 4) {
    decode_word();
    if (last_word_.parity_ok && arinc429_cb_) {
      arinc429_cb_(this, arinc429_user_data_);
    }
    buf_idx_ = 0;
  }
}

void Fl_ARINC429::feed_raw_word(uint32_t w) {
  buffer_[0] = w & 0xFF;
  buffer_[1] = (w >> 8) & 0xFF;
  buffer_[2] = (w >> 16) & 0xFF;
  buffer_[3] = (w >> 24) & 0xFF;
  decode_word();
  if (last_word_.parity_ok && arinc429_cb_) {
    arinc429_cb_(this, arinc429_user_data_);
  }
  buf_idx_ = 0;
}

bool Fl_ARINC429::check_parity(uint32_t raw_word)
{
  // ARINC 429 uses odd parity
  int ones = 0;
  for (int i = 0; i < 32; i++) {
    if (raw_word & (1U << i)) {
      ones++;
    }
  }
  return (ones % 2) != 0;
}

void Fl_ARINC429::decode_word() {
  uint32_t raw_word = ((uint32_t)buffer_[3] << 24) | 
                      ((uint32_t)buffer_[2] << 16) | 
                      ((uint32_t)buffer_[1] << 8) | 
                      (uint32_t)buffer_[0];
                      
  last_word_.parity_ok = check_parity(raw_word);
  
  // Bits 1-8: Label
  // Label in ARINC 429 is usually transmitted MSB first for the octal value,
  // but it's physically the first 8 bits. We'll extract them as the lower 8 bits.
  last_word_.label = raw_word & 0xFF;
  
  // Bits 9-10: SDI
  last_word_.sdi = (raw_word >> 8) & 0x03;
  
  // Bits 11-29: Data
  last_word_.data = (raw_word >> 10) & 0x7FFFF;
  
  // Bits 30-31: SSM
  last_word_.ssm = (raw_word >> 29) & 0x03;
}
