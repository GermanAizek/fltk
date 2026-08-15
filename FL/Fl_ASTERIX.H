//
// Eurocontrol / ICAO ASTERIX Surveillance Protocol class for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#ifndef Fl_ASTERIX_H
#define Fl_ASTERIX_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>
#include <stdint.h>
#include <stddef.h>

class Fl_ASTERIX;
typedef void (*Fl_ASTERIX_Callback)(Fl_ASTERIX*, void*);

/**
  Cross-platform Eurocontrol ASTERIX Surveillance Protocol Parser.
  
  Parses ASTERIX Data Blocks and Records:
  - Category (e.g. Cat 021 ADS-B Messages, Cat 048 Monoradar Targets, Cat 008 WXR)
  - SAC / SIC (System Area Code / System Identification Code)
  - Target Address (24-bit ICAO airframe hex)
  - Time of Day (1/128 sec units)
  - WGS-84 Latitude & Longitude
  - Flight Level (FL / Barometric Altitude)
  - Ground Speed & Track Angle
  - Mode 3/A Squawk Code & Callsign
*/
class FL_EXPORT Fl_ASTERIX : public Fl_Serial_Port {
public:
  struct Cat021Record {
    // 64-bit floating point (8-byte aligned)
    double   latitude_deg;    // WGS-84 Latitude
    double   longitude_deg;   // WGS-84 Longitude
    double   flight_level;    // Flight Level (1/4 FL or feet)
    double   ground_speed_kt; // Ground Speed in knots
    double   track_angle_deg; // Track angle in degrees

    // 32-bit & 16-bit integers
    uint32_t target_address;  // 24-bit ICAO address
    uint16_t squawk_mode3a;   // Mode 3/A code (e.g. 07700)

    // 8-bit metrics & strings
    uint8_t  category;        // 21 for ADS-B
    uint8_t  sac;             // System Area Code
    uint8_t  sic;             // System Identification Code
    bool     is_valid;
    char     callsign[10];    // 8-character ICAO callsign
  };

private:
  Cat021Record last_record_;
  Fl_ASTERIX_Callback asterix_cb_;
  void* user_data_;
  size_t buf_idx_;
  uint8_t buffer_[2048];

  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);

public:
  Fl_ASTERIX();
  virtual ~Fl_ASTERIX();

  int open(const char* port_name);
  void asterix_callback(Fl_ASTERIX_Callback cb, void* user_data = nullptr);

  const Cat021Record& last_record() const { return last_record_; }

  void feed_cat021(uint8_t sac, uint8_t sic, uint32_t icao24, double lat, double lon, double fl, double spd, double trk, uint16_t sqk, const char* cs);
};

#endif
