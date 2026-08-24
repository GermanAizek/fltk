//
// Mode-S / ADS-B 1090ES (DO-260B) Transponder Protocol implementation for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#include <FL/Fl_ADSB_1090ES.H>
#include <string.h>
#include <stdio.h>

Fl_ADSB_1090ES::Fl_ADSB_1090ES()
  : adsb_cb_(nullptr), user_data_(nullptr), buf_idx_(0) {
  memset(&last_msg_, 0, sizeof(last_msg_));
  memset(buffer_, 0, sizeof(buffer_));
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_ADSB_1090ES::~Fl_ADSB_1090ES() {
}

int Fl_ADSB_1090ES::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) return -1;
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  return 0;
}

void Fl_ADSB_1090ES::adsb_callback(Fl_ADSB_1090ES_Callback cb, void* user_data) {
  adsb_cb_ = cb;
  user_data_ = user_data;
}

void Fl_ADSB_1090ES::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_ADSB_1090ES* self = (Fl_ADSB_1090ES*)data;
  uint8_t buf[128];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      self->process_byte(buf[i]);
    }
  }
}

void Fl_ADSB_1090ES::process_byte(uint8_t b) {
  buffer_[buf_idx_++] = b;
  if (buf_idx_ == 14) {
    // 112-bit message received
    last_msg_.df = (buffer_[0] >> 3) & 0x1F;
    last_msg_.ca = buffer_[0] & 0x07;
    last_msg_.icao24 = (buffer_[1] << 16) | (buffer_[2] << 8) | buffer_[3];
    last_msg_.type_code = (buffer_[4] >> 3) & 0x1F;
    last_msg_.crc_valid = (calculate_crc24(buffer_, 14) == 0);

    if (adsb_cb_) adsb_cb_(this, user_data_);
    buf_idx_ = 0;
  }
}

uint32_t Fl_ADSB_1090ES::calculate_crc24(const uint8_t* msg, int len_bytes)
{
  static const uint32_t MODES_GENERATOR_POLY = 0x1FFF409;
  uint32_t crc = 0;
  for (int i = 0; i < len_bytes; i++) {
    crc ^= ((uint32_t)msg[i]) << 16;
    for (int bit = 0; bit < 8; bit++) {
      if (crc & 0x800000) {
        crc = (crc << 1) ^ MODES_GENERATOR_POLY;
      } else {
        crc = (crc << 1);
      }
    }
  }
  return crc & 0xFFFFFF;
}

void Fl_ADSB_1090ES::feed_raw_hex(const char* hex_14_bytes) {
  if (!hex_14_bytes || strlen(hex_14_bytes) < 28) return;
  for (int i = 0; i < 14; i++) {
    unsigned int val = 0;
    sscanf(hex_14_bytes + (i * 2), "%02x", &val);
    buffer_[i] = (uint8_t)val;
  }
  buf_idx_ = 14;
  process_byte(buffer_[13]);
}

void Fl_ADSB_1090ES::feed_squitter(uint32_t icao24, const char* callsign, double alt, double spd, double hdg, double vsi, uint16_t squawk) {
  last_msg_.df = 17;
  last_msg_.ca = 5;
  last_msg_.icao24 = icao24 & 0xFFFFFF;
  last_msg_.type_code = 19;
  last_msg_.altitude_ft = alt;
  last_msg_.speed_kts = spd;
  last_msg_.heading_deg = hdg;
  last_msg_.vsi_fpm = vsi;
  last_msg_.squawk = squawk;
  last_msg_.crc_valid = true;
  if (callsign) {
    snprintf(last_msg_.callsign, sizeof(last_msg_.callsign), "%s", callsign);
  } else {
    last_msg_.callsign[0] = '\0';
  }

  if (adsb_cb_) {
    adsb_cb_(this, user_data_);
  }
}
