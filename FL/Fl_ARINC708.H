//
// ARINC 708 Weather Radar (WXR) Protocol class for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#ifndef Fl_ARINC708_H
#define Fl_ARINC708_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>
#include <stdint.h>
#include <stddef.h>

class Fl_ARINC708;
typedef void (*Fl_ARINC708_Callback)(Fl_ARINC708*, void*);

/**
  Cross-platform ARINC 708 Weather Radar Antenna interface decoder.
  
  Decodes 1600-bit Manchester radar sweep radial pulses:
  - 64-bit Control Field (Scan angle, tilt angle, range selection, mode, gain)
  - 512 3-bit Range Bin reflectivity samples (0: Black, 1: Green, 2: Yellow, 3: Red, 4: Magenta/Turbulence)
  - 16-bit CRC / Parity verification
*/
class FL_EXPORT Fl_ARINC708 : public Fl_Serial_Port {
public:
  struct RadarRadial {
    float    scan_angle_deg;  // -90.0 to +90.0 degrees
    float    tilt_angle_deg;  // -15.0 to +15.0 degrees
    uint16_t range_nm;        // 10, 20, 40, 80, 160, 320 NM
    uint8_t  mode;            // WX, WX+TURB, MAP, TEST
    uint8_t  gain;            // Gain level
    uint8_t  range_bins[512]; // 0-7 color index for each range bin
    bool     is_valid;
  };

private:
  uint8_t buffer_[256];
  size_t buf_idx_;
  RadarRadial last_radial_;

  Fl_ARINC708_Callback arinc708_cb_;
  void* user_data_;

  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);

public:
  Fl_ARINC708();
  virtual ~Fl_ARINC708();

  int open(const char* port_name);
  void arinc708_callback(Fl_ARINC708_Callback cb, void* user_data = nullptr);

  const RadarRadial& last_radial() const { return last_radial_; }

  void feed_radial(float scan_angle, float tilt, uint16_t range, uint8_t mode, const uint8_t* bins, size_t bin_count);
};

#endif
