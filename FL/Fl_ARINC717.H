//
// ARINC 717 / ARINC 573 Digital Flight Data Recorder Protocol class for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#ifndef Fl_ARINC717_H
#define Fl_ARINC717_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>
#include <stdint.h>
#include <stddef.h>
#include <vector>

class Fl_ARINC717;
typedef void (*Fl_ARINC717_Callback)(Fl_ARINC717*, void*);

/**
  Cross-platform ARINC 717 / ARINC 573 flight data recorder (DFDR/FDAU) stream decoder.
  
  Decodes continuous 12-bit word subframes with standard sync patterns:
  - Subframe 1: 0x247
  - Subframe 2: 0x5B8
  - Subframe 3: 0xA47
  - Subframe 4: 0xDB8
  Supports word rates of 64, 128, 256, or 512 words per second.
*/
class FL_EXPORT Fl_ARINC717 : public Fl_Serial_Port {
public:
  struct Subframe {
    size_t word_count;           // 64, 128, 256, 512
    std::vector<uint16_t> words; // 12-bit telemetry words
    uint16_t sync_word;          // 0x247, 0x5B8, 0xA47, 0xDB8
    uint8_t subframe_num;        // 1 to 4
    bool is_valid;
  };

private:
  Subframe current_subframe_;
  size_t current_word_idx_;
  size_t raw_bit_count_;
  Fl_ARINC717_Callback arinc717_cb_;
  void* user_data_;
  uint8_t raw_bit_buf_[2];

  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);

public:
  Fl_ARINC717();
  virtual ~Fl_ARINC717();

  int open(const char* port_name);
  void arinc717_callback(Fl_ARINC717_Callback cb, void* user_data = nullptr);
  const Subframe& last_subframe() const { return current_subframe_; }

  void feed_raw_word(uint16_t word_12bit);
  void feed_subframe(uint8_t sf_num, const uint16_t* words, size_t count);
};

#endif
