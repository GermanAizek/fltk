//
// Eurocontrol / ICAO ASTERIX Surveillance Protocol implementation for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#include <FL/Fl_ASTERIX.H>
#include <string.h>
#include <stdio.h>

Fl_ASTERIX::Fl_ASTERIX()
  : asterix_cb_(nullptr), user_data_(nullptr), buf_idx_(0) {
  memset(&last_record_, 0, sizeof(last_record_));
  memset(buffer_, 0, sizeof(buffer_));
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_ASTERIX::~Fl_ASTERIX() {
}

int Fl_ASTERIX::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) return -1;
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  return 0;
}

void Fl_ASTERIX::asterix_callback(Fl_ASTERIX_Callback cb, void* user_data) {
  asterix_cb_ = cb;
  user_data_ = user_data;
}

void Fl_ASTERIX::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_ASTERIX* self = (Fl_ASTERIX*)data;
  uint8_t buf[128];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      self->process_byte(buf[i]);
    }
  }
}

void Fl_ASTERIX::process_byte(uint8_t b) {
  if (buf_idx_ == 0 && b != 21 && b != 48 && b != 8) return; // Support Cat 021, 048, 008
  buffer_[buf_idx_++] = b;

  if (buf_idx_ >= 3) {
    uint16_t block_len = (buffer_[1] << 8) | buffer_[2];
    if (block_len < 3 || block_len > 2048) {
      buf_idx_ = 0;
      return;
    }
    if (buf_idx_ == block_len) {
      // Decode ASTERIX Data Block
      last_record_.category = buffer_[0];
      last_record_.sac = buffer_[3];
      last_record_.sic = buffer_[4];
      last_record_.is_valid = true;
      if (asterix_cb_) asterix_cb_(this, user_data_);
      buf_idx_ = 0;
    }
  }
}

void Fl_ASTERIX::feed_cat021(uint8_t sac, uint8_t sic, uint32_t icao24, double lat, double lon, double fl, double spd, double trk, uint16_t sqk, const char* cs) {
  last_record_.category = 21;
  last_record_.sac = sac;
  last_record_.sic = sic;
  last_record_.target_address = icao24 & 0xFFFFFF;
  last_record_.latitude_deg = lat;
  last_record_.longitude_deg = lon;
  last_record_.flight_level = fl;
  last_record_.ground_speed_kt = spd;
  last_record_.track_angle_deg = trk;
  last_record_.squawk_mode3a = sqk;
  if (cs) {
    snprintf(last_record_.callsign, sizeof(last_record_.callsign), "%s", cs);
  } else {
    last_record_.callsign[0] = '\0';
  }
  last_record_.is_valid = true;

  if (asterix_cb_) {
    asterix_cb_(this, user_data_);
  }
}
