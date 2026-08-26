//
// AFDX (ARINC 664) Protocol class for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_AFDX.H>
#include <string.h>

#define SLIP_END     0xC0
#define SLIP_ESC     0xDB
#define SLIP_ESC_END 0xDC
#define SLIP_ESC_ESC 0xDD

Fl_AFDX::Fl_AFDX() : afdx_cb_(nullptr), afdx_user_data_(nullptr), escape_flag_(false) {
  last_frame_.payload_length = 0;
  last_frame_.virtual_link = 0;
  last_frame_.seq_num = 0;
  last_frame_.is_valid = false;
  
  // Register the internal serial callback
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_AFDX::~Fl_AFDX() {
}

int Fl_AFDX::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) {
    return -1;
  }
  
  // Default to a high baud rate suitable for Ethernet encapsulation
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  
  return 0;
}

void Fl_AFDX::afdx_callback(Fl_AFDX_Callback cb, void* user_data) {
  afdx_cb_ = cb;
  afdx_user_data_ = user_data;
}

void Fl_AFDX::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_AFDX* afdx = (Fl_AFDX*)data;
  uint8_t buf[256];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      afdx->process_byte(buf[i]);
    }
  }
}

void Fl_AFDX::process_byte(uint8_t b) {
  if (escape_flag_) {
    if (b == SLIP_ESC_END) {
      b = SLIP_END;
    } else if (b == SLIP_ESC_ESC) {
      b = SLIP_ESC;
    }
    escape_flag_ = false;
  } else {
    if (b == SLIP_ESC) {
      escape_flag_ = true;
      return;
    } else if (b == SLIP_END) {
      if (!buffer_.empty()) {
        decode_frame(buffer_.data(), (int)buffer_.size());
        if (last_frame_.is_valid && afdx_cb_) {
          afdx_cb_(this, afdx_user_data_);
        }
        buffer_.clear();
      }
      return;
    }
  }

  if (buffer_.size() < 65536) {
    buffer_.push_back(b);
  } else {
    // Buffer overflow, drop the frame and reset
    buffer_.clear();
  }
}

void Fl_AFDX::feed_raw_frame(const uint8_t* data, int len) {
  if (len > 0) {
    buffer_.assign(data, data + len);
    decode_frame(buffer_.data(), len);
    if (last_frame_.is_valid && afdx_cb_) {
      afdx_cb_(this, afdx_user_data_);
    }
  }
}

void Fl_AFDX::decode_frame(const uint8_t* frame, int length) {
  last_frame_.is_valid = false;
  
  // Minimum Ethernet frame length is usually 64 bytes, but AFDX frames
  // need at least MACs (14) + IP (20) + UDP (8) + SEQ (1) = 43 bytes
  if (length < 43) return;
  
  // AFDX Destination MAC format: 03:00:00:00:XX:YY where XX:YY is the Virtual Link ID
  if (frame[0] == 0x03 && frame[1] == 0x00 && frame[2] == 0x00 && frame[3] == 0x00) {
    last_frame_.virtual_link = ((uint16_t)frame[4] << 8) | frame[5];
    last_frame_.is_valid = true;
  }
  
  // In AFDX, the sequence number is a single byte placed at the end of the UDP payload.
  // We assume the received frame length includes the MAC header up to the Sequence Number.
  // We extract the last byte.
  last_frame_.seq_num = frame[length - 1];
  
  // The payload length is roughly the total length minus MACs(14) and SEQ(1)
  last_frame_.payload_length = length - 15; 
}
