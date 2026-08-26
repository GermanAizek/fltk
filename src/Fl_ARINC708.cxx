//
// ARINC 708 Weather Radar (WXR) Protocol implementation for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#include <FL/Fl_ARINC708.H>
#include <string.h>

Fl_ARINC708::Fl_ARINC708()
  : arinc708_cb_(nullptr), user_data_(nullptr) {
  last_radial_.scan_angle_deg = 0.0f;
  last_radial_.tilt_angle_deg = 0.0f;
  last_radial_.range_nm = 0;
  last_radial_.mode = 0;
  last_radial_.gain = 0;
  last_radial_.is_valid = false;
  Fl_Serial_Port::callback(serial_cb, this);
}

Fl_ARINC708::~Fl_ARINC708() {
}

int Fl_ARINC708::open(const char* port_name) {
  if (Fl_Serial_Port::open(port_name) != 0) return -1;
  set_baud_rate(115200);
  set_data_bits(DATA_8);
  set_parity(PARITY_NONE);
  set_stop_bits(STOP_1);
  return 0;
}

void Fl_ARINC708::arinc708_callback(Fl_ARINC708_Callback cb, void* user_data) {
  arinc708_cb_ = cb;
  user_data_ = user_data;
}

void Fl_ARINC708::serial_cb(Fl_Serial_Port* p, void* data) {
  Fl_ARINC708* self = (Fl_ARINC708*)data;
  uint8_t buf[128];
  int bytes_read = p->read_data(buf, sizeof(buf));
  if (bytes_read > 0) {
    for (int i = 0; i < bytes_read; i++) {
      self->process_byte(buf[i]);
    }
  }
}

void Fl_ARINC708::process_byte(uint8_t b) {
  if (buffer_.empty() && b != 0x70) return;
  if (buffer_.size() == 1 && b != 0x88) { buffer_.clear(); return; }

  buffer_.push_back(b);

  // ARINC 708 raw packet payload (200 bytes for 1600 bits)
  if (buffer_.size() == 200) {
    int16_t raw_scan = (buffer_[2] << 8) | buffer_[3];
    int16_t raw_tilt = (buffer_[4] << 8) | buffer_[5];
    uint16_t range = (buffer_[6] << 8) | buffer_[7];
    uint8_t mode = buffer_[8];

    feed_radial((float)raw_scan * 0.1f, (float)raw_tilt * 0.1f, range, mode, buffer_.data() + 10, 188);
    buffer_.clear();
  }
}

void Fl_ARINC708::feed_radial(float scan_angle, float tilt, uint16_t range, uint8_t mode, const uint8_t* bins, size_t bin_count) {
  last_radial_.scan_angle_deg = scan_angle;
  last_radial_.tilt_angle_deg = tilt;
  last_radial_.range_nm = range;
  last_radial_.mode = mode;
  last_radial_.gain = 100;
  last_radial_.is_valid = true;

  size_t copy_bins = (bin_count > 512) ? 512 : bin_count;
  if (bins && copy_bins > 0) {
    last_radial_.range_bins.assign(bins, bins + copy_bins);
  } else {
    last_radial_.range_bins.clear();
  }

  if (arinc708_cb_) {
    arinc708_cb_(this, user_data_);
  }
}
