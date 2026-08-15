//
// Mode-S / ADS-B 1090ES (DO-260B) Transponder Protocol class for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#ifndef Fl_ADSB_1090ES_H
#define Fl_ADSB_1090ES_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>
#include <stdint.h>
#include <stddef.h>

class Fl_ADSB_1090ES;
typedef void (*Fl_ADSB_1090ES_Callback)(Fl_ADSB_1090ES*, void*);

/**
  Cross-platform Mode-S / ADS-B 1090 MHz Extended Squitter Decoder (DO-260B).
  
  Decodes 112-bit Extended Squitter Downlink Formats:
  - DF17 / DF18 Extended Squitter
  - 24-bit ICAO Aircraft Address
  - Type Code (TC):
      TC 1-4: Aircraft Identification / Callsign
      TC 9-18: Airborne Position (Compact Position Reporting - CPR odd/even)
      TC 19: Airborne Velocity (East/West, North/South, Vertical Rate)
      TC 28: Emergency Status / Squawk
  - CRC-24 (Polynomial 0x1FFF409) Syndrome checking
*/
class FL_EXPORT Fl_ADSB_1090ES : public Fl_Serial_Port {
public:
  struct SquitterMsg {
    // 64-bit doubles (8-byte aligned)
    double   altitude_ft;     // Decoded barometric / GNSS altitude
    double   lat_cpr;         // Encoded CPR latitude
    double   lon_cpr;         // Encoded CPR longitude
    double   speed_kts;       // Velocity in knots
    double   heading_deg;     // Heading / Track angle
    double   vsi_fpm;         // Vertical rate

    // 32-bit & 16-bit integers
    uint32_t icao24;          // 24-bit ICAO address
    uint16_t squawk;          // 4-digit octal squawk (e.g. 7700)

    // 8-bit metrics & strings
    uint8_t  df;              // Downlink Format (17 or 18)
    uint8_t  ca;              // Capability
    uint8_t  type_code;       // Extended Squitter Type Code (1-31)
    bool     crc_valid;
    char     callsign[10];    // Decoded callsign
  };

private:
  SquitterMsg last_msg_;
  Fl_ADSB_1090ES_Callback adsb_cb_;
  void* user_data_;
  size_t buf_idx_;
  uint8_t buffer_[14]; // 112 bits = 14 bytes

  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);
  uint32_t calculate_crc24(const uint8_t* msg, int len_bytes) const;

public:
  Fl_ADSB_1090ES();
  virtual ~Fl_ADSB_1090ES();

  int open(const char* port_name);
  void adsb_callback(Fl_ADSB_1090ES_Callback cb, void* user_data = nullptr);

  const SquitterMsg& last_message() const { return last_msg_; }

  void feed_raw_hex(const char* hex_14_bytes);
  void feed_squitter(uint32_t icao24, const char* callsign, double alt, double spd, double hdg, double vsi, uint16_t squawk);
};

#endif
