//
// ARINC 717 / ARINC 573 Digital Flight Data Recorder Protocol implementation for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#include <FL/Fl_ARINC717.H>
#include <string.h>

Fl_ARINC717::Fl_ARINC717() 
  : current_word_idx_(0), raw_bit_count_(0), arinc717_cb_(nullptr), user_data_(nullptr) {
  current_subframe_.word_count = 0;
  current_subframe_.sync_word = 0;
  current_subframe_.subframe_num = 0;
  current_subframe_.is_valid = false;
  raw_bit_buf_[0] = 0;
  raw_bit_buf_[1] = 0;
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_ARINC717::~Fl_ARINC717() {
}

int Fl_ARINC717::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) return -1;
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  return 0;
}

void Fl_ARINC717::arinc717_callback(Fl_ARINC717_Callback cb, void* user_data) {
  arinc717_cb_ = cb;
  user_data_ = user_data;
}

void Fl_ARINC717::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_ARINC717* self = (Fl_ARINC717*)data;
  uint8_t buf[128];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      self->process_byte(buf[i]);
    }
  }
}

void Fl_ARINC717::process_byte(uint8_t b) {
  if (raw_bit_count_ < 2) {
    raw_bit_buf_[raw_bit_count_++] = b;
  }
  if (raw_bit_count_ >= 2) {
    uint16_t w = ((uint16_t)raw_bit_buf_[0] << 8) | raw_bit_buf_[1];
    w &= 0x0FFF; // 12 bits
    raw_bit_count_ = 0;
    feed_raw_word(w);
  }
}

void Fl_ARINC717::feed_raw_word(uint16_t word_12bit) {
  uint16_t sync = word_12bit & 0x0FFF;
  if (sync == 0x247 || sync == 0x5B8 || sync == 0xA47 || sync == 0xDB8) {
    // New subframe detected
    if (!current_subframe_.words.empty() && arinc717_cb_) {
      current_subframe_.word_count = current_subframe_.words.size();
      current_subframe_.is_valid = true;
      arinc717_cb_(this, user_data_);
    }
    
    current_word_idx_ = 0;
    current_subframe_.words.clear();
    current_subframe_.sync_word = sync;
    if (sync == 0x247) current_subframe_.subframe_num = 1;
    else if (sync == 0x5B8) current_subframe_.subframe_num = 2;
    else if (sync == 0xA47) current_subframe_.subframe_num = 3;
    else if (sync == 0xDB8) current_subframe_.subframe_num = 4;
    
    current_subframe_.words.push_back(sync);
  } else {
    if (current_subframe_.words.size() < 512) {
      current_subframe_.words.push_back(word_12bit & 0x0FFF);
    }
  }
}

void Fl_ARINC717::feed_subframe(uint8_t sf_num, const uint16_t* words, size_t count) {
  if (!words || count == 0) return;
  current_subframe_.subframe_num = sf_num;
  if (sf_num == 1) current_subframe_.sync_word = 0x247;
  else if (sf_num == 2) current_subframe_.sync_word = 0x5B8;
  else if (sf_num == 3) current_subframe_.sync_word = 0xA47;
  else current_subframe_.sync_word = 0xDB8;

  size_t copy_cnt = (count > 512) ? 512 : count;
  current_subframe_.words.resize(copy_cnt);
  current_subframe_.words[0] = current_subframe_.sync_word;
  for (size_t i = 1; i < copy_cnt; i++) {
    current_subframe_.words[i] = words[i] & 0x0FFF;
  }
  current_subframe_.word_count = copy_cnt;
  current_subframe_.is_valid = true;

  if (arinc717_cb_) {
    arinc717_cb_(this, user_data_);
  }
}
