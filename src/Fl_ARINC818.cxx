//
// ARINC 818 (ADVB - Avionics Digital Video Bus) Protocol implementation for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#include <FL/Fl_ARINC818.H>
#include <string.h>

Fl_ARINC818::Fl_ARINC818()
  : arinc818_cb_(nullptr), user_data_(nullptr), buf_idx_(0) {
  memset(&last_container_, 0, sizeof(last_container_));
  memset(buffer_, 0, sizeof(buffer_));
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_ARINC818::~Fl_ARINC818() {
}

int Fl_ARINC818::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) return -1;
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  return 0;
}

void Fl_ARINC818::arinc818_callback(Fl_ARINC818_Callback cb, void* user_data) {
  arinc818_cb_ = cb;
  user_data_ = user_data;
}

void Fl_ARINC818::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_ARINC818* self = (Fl_ARINC818*)data;
  uint8_t buf[128];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      self->process_byte(buf[i]);
    }
  }
}

void Fl_ARINC818::process_byte(uint8_t b) {
  if (buf_idx_ == 0 && b != 0x81) return;
  if (buf_idx_ == 1 && b != 0x88) { buf_idx_ = 0; return; }

  buffer_[buf_idx_++] = b;

  if (buf_idx_ >= 16) {
    uint16_t w = (buffer_[4] << 8) | buffer_[5];
    uint16_t h = (buffer_[6] << 8) | buffer_[7];
    uint16_t fps = (buffer_[8] << 8) | buffer_[9];
    uint32_t fnum = (buffer_[10] << 24) | (buffer_[11] << 16) | (buffer_[12] << 8) | buffer_[13];
    uint16_t anclen = (buffer_[14] << 8) | buffer_[15];

    if (buf_idx_ == (size_t)(16 + anclen)) {
      feed_container(w, h, fps, fnum, buffer_ + 16, anclen);
      buf_idx_ = 0;
    }
  }
}

void Fl_ARINC818::feed_container(uint16_t width, uint16_t height, uint16_t fps, uint32_t frame_num, const uint8_t* ancillary, uint16_t anc_len) {
  last_container_.container_ver = 2;
  last_container_.color_format = 0; // RGB
  last_container_.video_width = width;
  last_container_.video_height = height;
  last_container_.frame_rate_fps = fps;
  last_container_.frame_count = frame_num;
  last_container_.is_valid = true;

  last_container_.ancillary_len = (anc_len > sizeof(last_container_.ancillary_data)) ? sizeof(last_container_.ancillary_data) : anc_len;
  if (ancillary && last_container_.ancillary_len > 0) {
    memcpy(last_container_.ancillary_data, ancillary, last_container_.ancillary_len);
  }

  if (arinc818_cb_) {
    arinc818_cb_(this, user_data_);
  }
}
