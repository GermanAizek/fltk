//
// ARINC 429 Protocol class for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     https://www.fltk.org/COPYING.php
//
// Please see the following page on how to report bugs and issues:
//
//     https://www.fltk.org/bugs.php
//

#ifndef Fl_ARINC429_H
#define Fl_ARINC429_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>

class Fl_ARINC429;
typedef void (*Fl_ARINC429_Callback)(Fl_ARINC429*, void*);

/**
  Cross-platform ARINC 429 protocol wrapper.
  
  Inherits from Fl_Serial_Port to provide decoding of 32-bit ARINC 429 words.
  The typical ARINC 429 baud rates are 12.5 kbps (low speed) or 100 kbps (high speed).
  This class reads 4 bytes at a time, checks odd parity, and extracts the Label,
  SDI, Data, and SSM fields.
  
  The parser maintains an internal buffer and triggers a user-defined callback
  whenever a valid complete 32-bit word is successfully parsed.
*/
class FL_EXPORT Fl_ARINC429 : public Fl_Serial_Port {
public:
  /**
    Structure representing a decoded ARINC 429 word.
  */
  struct Word {
    uint32_t data;    // 19 bits
    uint8_t label;    // 8 bits
    uint8_t sdi;      // 2 bits
    uint8_t ssm;      // 2 bits
    bool parity_ok;
  };

private:
  Word last_word_;
  Fl_ARINC429_Callback arinc429_cb_;
  void* arinc429_user_data_;
  uint8_t buffer_[4];
  uint8_t buf_idx_;

  // Internal serial callback to feed the parser
  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);
  void decode_word();
  bool check_parity(uint32_t raw_word) const;

public:
  Fl_ARINC429();
  virtual ~Fl_ARINC429();

  /**
    Opens the serial port.
    \param port_name The path or name of the port.
    \return 0 on success, or -1 on error.
  */
  int open(const char* port_name);

  /**
    Registers a callback function to be called when a valid 32-bit ARINC 429 word is received.
    \param cb The callback function.
    \param user_data Optional user data.
  */
  void arinc429_callback(Fl_ARINC429_Callback cb, void* user_data = nullptr);

  /**
    Gets the last received and decoded ARINC 429 word.
    \return The last decoded word structure.
  */
  const Word& last_word() const { return last_word_; }
  
  // For unit testing: manually feed a byte into the parser
  void feed_byte(uint8_t b) { process_byte(b); }
  
  // For unit testing: manually feed a 32-bit raw word
  void feed_raw_word(uint32_t w);
};

#endif
