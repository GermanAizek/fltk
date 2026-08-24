//
// MIL-STD-1553 Protocol class for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_MIL_STD_1553.H>
#include <string.h>

Fl_MIL_STD_1553::Fl_MIL_STD_1553() : mil1553_cb_(nullptr), mil1553_user_data_(nullptr), buf_idx_(0) {
  memset(buffer_, 0, sizeof(buffer_));
  memset(&last_word_, 0, sizeof(last_word_));
  
  // Register the internal serial callback
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_MIL_STD_1553::~Fl_MIL_STD_1553() {
}

int Fl_MIL_STD_1553::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) {
    return -1;
  }
  
  // Reasonable default for an MIL-STD-1553 serial interface adapter
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  
  return 0;
}

void Fl_MIL_STD_1553::mil_std_1553_callback(Fl_MIL_STD_1553_Callback cb, void* user_data) {
  mil1553_cb_ = cb;
  mil1553_user_data_ = user_data;
}

void Fl_MIL_STD_1553::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_MIL_STD_1553* mil = (Fl_MIL_STD_1553*)data;
  uint8_t buf[64];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      mil->process_byte(buf[i]);
    }
  }
}

void Fl_MIL_STD_1553::process_byte(uint8_t b) {
  buffer_[buf_idx_++] = b;
  if (buf_idx_ == 3) {
    decode_word();
    if (last_word_.parity_ok && mil1553_cb_) {
      mil1553_cb_(this, mil1553_user_data_);
    }
    buf_idx_ = 0;
  }
}

void Fl_MIL_STD_1553::feed_raw_word(uint32_t w) {
  buffer_[0] = w & 0xFF;
  buffer_[1] = (w >> 8) & 0xFF;
  buffer_[2] = (w >> 16) & 0xFF;
  decode_word();
  if (last_word_.parity_ok && mil1553_cb_) {
    mil1553_cb_(this, mil1553_user_data_);
  }
  buf_idx_ = 0;
}

bool Fl_MIL_STD_1553::check_parity(uint32_t raw_20bit)
{
  // MIL-STD-1553 uses odd parity across the 16 data bits and 1 parity bit
  // The sync bits are typically not included in parity.
  // We'll calculate odd parity on the 17 bits (16 data + 1 parity).
  int ones = 0;
  for (int i = 0; i < 17; i++) {
    if (raw_20bit & (1U << i)) {
      ones++;
    }
  }
  return (ones % 2) != 0;
}

void Fl_MIL_STD_1553::decode_word() {
  // Assuming a 24-bit package: [8 bits unused/status] [3 bits sync] [16 bits data] [1 bit parity]
  // In our 3-byte buffer:
  // buffer_[0]: data low (8 bits)
  // buffer_[1]: data high (8 bits)
  // buffer_[2]: bit 0=parity, bit 1-3=sync, upper bits=status
  
  uint32_t raw_word = ((uint32_t)buffer_[2] << 16) | 
                      ((uint32_t)buffer_[1] << 8) | 
                      (uint32_t)buffer_[0];
                      
  // Extract 16 data bits and 1 parity bit for parity check
  // data is in bits 0-15
  // parity is in bit 16
  uint32_t parity_check_word = raw_word & 0x1FFFF;
  last_word_.parity_ok = check_parity(parity_check_word);
  
  // 16 bits: Data
  last_word_.data = raw_word & 0xFFFF;
  
  // Sync pattern is in bits 17-19
  // We map: 0 => Command/Status Sync, 1 => Data Sync
  uint8_t sync = (raw_word >> 17) & 0x07;
  last_word_.sync = (sync == 1) ? SYNC_DATA : SYNC_COMMAND_STATUS;
}
