//
// FPort Protocol class for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_FPort.H>
#include <string.h>

Fl_FPort::Fl_FPort() : fport_cb_(nullptr), fport_user_data_(nullptr),
                       state_(WAIT_SYNC), frame_length_(0), frame_type_(0), 
                       buf_idx_(0), flags_(0), rssi_(0), escape_next_(false) {
  memset(channels_, 0, sizeof(channels_));
  memset(buffer_, 0, sizeof(buffer_));
  
  // Register the internal serial callback
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_FPort::~Fl_FPort() {
}

int Fl_FPort::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) {
    return -1;
  }
  
  // Configure for FPort: 115200 baud, 8N1
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  
  return 0;
}

void Fl_FPort::fport_callback(Fl_FPort_Callback cb, void* user_data) {
  fport_cb_ = cb;
  fport_user_data_ = user_data;
}

uint16_t Fl_FPort::channel(int ch) const {
  if (ch >= 1 && ch <= 16) {
    return channels_[ch - 1];
  }
  return 0;
}

void Fl_FPort::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_FPort* fport = (Fl_FPort*)data;
  uint8_t buf[256];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      fport->process_byte(buf[i]);
    }
  }
}

void Fl_FPort::process_byte(uint8_t b) {
  if (b == 0x7E) {
    // Start of frame
    state_ = WAIT_LENGTH;
    escape_next_ = false;
    return;
  }
  
  if (state_ == WAIT_SYNC) {
    return;
  }
  
  // Handle byte stuffing
  if (b == 0x7D) {
    escape_next_ = true;
    return;
  }
  if (escape_next_) {
    b ^= 0x20;
    escape_next_ = false;
  }
  
  switch (state_) {
    case WAIT_LENGTH:
      frame_length_ = b;
      state_ = WAIT_TYPE;
      break;
      
    case WAIT_TYPE:
      frame_type_ = b;
      buf_idx_ = 0;
      if (frame_length_ > 1) { // -1 for type
        state_ = WAIT_PAYLOAD;
      } else {
        state_ = WAIT_SYNC;
      }
      break;
      
    case WAIT_PAYLOAD:
      buffer_[buf_idx_++] = b;
      
      // Expected payload length: frame_length - 1 (type) + 1 (checksum)
      if (buf_idx_ >= frame_length_) {
        // Full packet received, check checksum
        uint16_t sum = frame_length_ + frame_type_;
        
        for (int i = 0; i < frame_length_ - 1; i++) {
          sum += buffer_[i];
        }
        
        // FrSky Checksum logic
        sum += sum >> 8;
        sum &= 0xFF;
        uint8_t checksum = 0xFF - sum;
        
        uint8_t received_checksum = buffer_[frame_length_ - 1];
        
        if (checksum == received_checksum) {
          if (frame_type_ == 0x00 && frame_length_ >= 25) { // Control (RC) frame
            decode_rc_frame();
            if (fport_cb_) {
              fport_cb_(this, fport_user_data_);
            }
          }
        }
        
        state_ = WAIT_SYNC;
      }
      break;
      
    default:
      state_ = WAIT_SYNC;
      break;
  }
}

void Fl_FPort::decode_rc_frame() {
  // FPort packs SBUS channels identically to SBUS (11 bits per channel)
  channels_[0]  = (buffer_[0]       | (buffer_[1] << 8))                      & 0x07FF;
  channels_[1]  = ((buffer_[1] >> 3) | (buffer_[2] << 5))                      & 0x07FF;
  channels_[2]  = ((buffer_[2] >> 6) | (buffer_[3] << 2) | (buffer_[4] << 10)) & 0x07FF;
  channels_[3]  = ((buffer_[4] >> 1) | (buffer_[5] << 7))                      & 0x07FF;
  channels_[4]  = ((buffer_[5] >> 4) | (buffer_[6] << 4))                      & 0x07FF;
  channels_[5]  = ((buffer_[6] >> 7) | (buffer_[7] << 1) | (buffer_[8] << 9))  & 0x07FF;
  channels_[6]  = ((buffer_[8] >> 2) | (buffer_[9] << 6))                      & 0x07FF;
  channels_[7]  = ((buffer_[9] >> 5) | (buffer_[10] << 3))                     & 0x07FF;
  channels_[8]  = (buffer_[11]       | (buffer_[12] << 8))                     & 0x07FF;
  channels_[9]  = ((buffer_[12] >> 3)| (buffer_[13] << 5))                     & 0x07FF;
  channels_[10] = ((buffer_[13] >> 6)| (buffer_[14] << 2)| (buffer_[15] << 10))& 0x07FF;
  channels_[11] = ((buffer_[15] >> 1)| (buffer_[16] << 7))                     & 0x07FF;
  channels_[12] = ((buffer_[16] >> 4)| (buffer_[17] << 4))                     & 0x07FF;
  channels_[13] = ((buffer_[17] >> 7)| (buffer_[18] << 1)| (buffer_[19] << 9)) & 0x07FF;
  channels_[14] = ((buffer_[19] >> 2)| (buffer_[20] << 6))                     & 0x07FF;
  channels_[15] = ((buffer_[20] >> 5)| (buffer_[21] << 3))                     & 0x07FF;
  
  // SBUS Flags
  flags_ = buffer_[22];
  
  // RSSI
  rssi_ = buffer_[23];
}
