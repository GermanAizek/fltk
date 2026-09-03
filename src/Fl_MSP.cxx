//
// MSP Protocol class for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_MSP.H>
#include <string.h>

Fl_MSP::Fl_MSP() : msp_cb_(nullptr), msp_user_data_(nullptr),
                   payload_buffer_(nullptr),
                   state_(WAIT_HEADER_1), payload_size_(0), cmd_(0), 
                   current_checksum_(0), buf_idx_(0) {
  // Register the internal serial callback
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_MSP::~Fl_MSP() {
  delete[] payload_buffer_;
}

int Fl_MSP::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) {
    return -1;
  }
  
  // Configure for MSP: 115200 baud, 8N1
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  
  return 0;
}

void Fl_MSP::msp_callback(Fl_MSP_Callback cb, void* user_data) {
  msp_cb_ = cb;
  msp_user_data_ = user_data;
}

void Fl_MSP::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_MSP* msp = (Fl_MSP*)data;
  uint8_t buf[256];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      msp->process_byte(buf[i]);
    }
  }
}

void Fl_MSP::process_byte(uint8_t b) {
  switch (state_) {
    case WAIT_HEADER_1:
      if (b == '$') {
        state_ = WAIT_HEADER_2;
      }
      break;
      
    case WAIT_HEADER_2:
      if (b == 'M') {
        state_ = WAIT_DIRECTION;
      } else {
        state_ = WAIT_HEADER_1;
      }
      break;
      
    case WAIT_DIRECTION:
      if (b == '<' || b == '>') {
        state_ = WAIT_SIZE;
      } else {
        state_ = WAIT_HEADER_1;
      }
      break;
      
    case WAIT_SIZE:
      payload_size_ = b;
      current_checksum_ = b;
      state_ = WAIT_CMD;
      break;
      
    case WAIT_CMD:
      cmd_ = b;
      current_checksum_ ^= b;
      buf_idx_ = 0;
      if (payload_size_ > 0) {
        if (!payload_buffer_) {
          payload_buffer_ = new uint8_t[256];
        }
        state_ = WAIT_PAYLOAD;
      } else {
        state_ = WAIT_CHECKSUM;
      }
      break;
      
    case WAIT_PAYLOAD:
      if (payload_buffer_) {
        payload_buffer_[buf_idx_++] = b;
      }
      current_checksum_ ^= b;
      if (buf_idx_ >= payload_size_) {
        state_ = WAIT_CHECKSUM;
      }
      break;
      
    case WAIT_CHECKSUM:
      if (b == current_checksum_) {
        // Valid frame received
        if (msp_cb_) {
          msp_cb_(this, cmd_, payload_buffer_, payload_size_, msp_user_data_);
        }
      }
      // Reset state for next frame
      state_ = WAIT_HEADER_1;
      break;
  }
}

void Fl_MSP::send_command(uint8_t cmd, const uint8_t* payload, uint8_t size) {
  uint8_t buf[262]; // Max size: 3 (header) + 1 (size) + 1 (cmd) + 255 (payload) + 1 (checksum) = 261
  int idx = 0;
  
  buf[idx++] = '$';
  buf[idx++] = 'M';
  buf[idx++] = '<'; // '<' for to-FC
  buf[idx++] = size;
  buf[idx++] = cmd;
  
  uint8_t checksum = size ^ cmd;
  
  for (int i = 0; i < size; i++) {
    buf[idx++] = payload[i];
    checksum ^= payload[i];
  }
  
  buf[idx++] = checksum;
  
  write_data(buf, idx);
}
