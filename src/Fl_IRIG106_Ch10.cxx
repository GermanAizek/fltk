//
// IRIG 106 Chapter 10 / 11 Flight Test Telemetry Protocol implementation for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#include <FL/Fl_IRIG106_Ch10.H>
#include <string.h>

Fl_IRIG106_Ch10::Fl_IRIG106_Ch10()
  : irig_cb_(nullptr), user_data_(nullptr), buf_idx_(0), payload_len_(0) {
  memset(&last_header_, 0, sizeof(last_header_));
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_IRIG106_Ch10::~Fl_IRIG106_Ch10() {
}

int Fl_IRIG106_Ch10::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) return -1;
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  return 0;
}

void Fl_IRIG106_Ch10::irig_callback(Fl_IRIG106_Callback cb, void* user_data) {
  irig_cb_ = cb;
  user_data_ = user_data;
}

void Fl_IRIG106_Ch10::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_IRIG106_Ch10* self = (Fl_IRIG106_Ch10*)data;
  uint8_t buf[256];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      self->process_byte(buf[i]);
    }
  }
}

void Fl_IRIG106_Ch10::process_byte(uint8_t b) {
  if (buf_idx_ == 0 && b != 0x25) return;
  if (buf_idx_ == 1 && b != 0xEB) { buf_idx_ = 0; buffer_.clear(); return; }

  buffer_.push_back(b);
  buf_idx_++;

  if (buf_idx_ >= 24) {
    uint32_t pkt_len = buffer_[4] | (buffer_[5] << 8) | (buffer_[6] << 16) | (buffer_[7] << 24);
    if (pkt_len < 24 || pkt_len > 1048576) {
      buf_idx_ = 0;
      buffer_.clear();
      return;
    }

    if (buf_idx_ == pkt_len) {
      feed_raw_packet(buffer_.data(), pkt_len);
      buf_idx_ = 0;
      buffer_.clear();
    }
  }
}

void Fl_IRIG106_Ch10::feed_raw_packet(const uint8_t* pkt, size_t len) {
  if (!pkt || len < 24) return;

  last_header_.sync = (pkt[1] << 8) | pkt[0];
  last_header_.channel_id = (pkt[3] << 8) | pkt[2];
  last_header_.packet_len = pkt[4] | (pkt[5] << 8) | (pkt[6] << 16) | (pkt[7] << 24);
  last_header_.data_len = pkt[8] | (pkt[9] << 8) | (pkt[10] << 16) | (pkt[11] << 24);
  last_header_.data_type = pkt[12];
  last_header_.seq_num = pkt[13];
  last_header_.packet_flags = pkt[14];
  last_header_.data_type_ver = pkt[15];
  
  uint64_t rtc = 0;
  for (int i = 0; i < 6; i++) {
    rtc |= ((uint64_t)pkt[16 + i] << (i * 8));
  }
  last_header_.rtc_timestamp = rtc;
  last_header_.header_checksum = (pkt[23] << 8) | pkt[22];

  uint16_t calc_sum = 0;
  for (size_t i = 0; i < 22; i += 2) {
    calc_sum += (pkt[i + 1] << 8) | pkt[i];
  }
  last_header_.checksum_ok = (last_header_.header_checksum == 0 || last_header_.header_checksum == calc_sum);

  payload_len_ = len - 24;
  if (payload_len_ > 0) {
    payload_.assign(pkt + 24, pkt + 24 + payload_len_);
  } else {
    payload_.clear();
  }

  if (irig_cb_) {
    irig_cb_(this, user_data_);
  }
}
