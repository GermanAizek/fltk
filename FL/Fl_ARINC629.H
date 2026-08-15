//
// ARINC 629 Protocol class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_ARINC629_H
#define Fl_ARINC629_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>

class Fl_ARINC629;
typedef void (*Fl_ARINC629_Callback)(Fl_ARINC629*, void*);

/**
  Cross-platform ARINC 629 protocol wrapper.
  
  Inherits from Fl_Serial_Port to provide decoding of ARINC 629 words.
  ARINC 629 typically transmits 20-bit words (3 bits sync, 16 bits data, 1 bit parity)
  at 2 Mbps over the physical bus. Over a serial adapter, this is often packaged
  into 3 or 4 bytes. Here we assume a 3-byte payload representing a 24-bit structure,
  where the 20 bits of the ARINC 629 word are encapsulated.
  
  The parser maintains an internal buffer and triggers a user-defined callback
  whenever a valid complete word is successfully parsed.
*/
class FL_EXPORT Fl_ARINC629 : public Fl_Serial_Port {
public:
  enum WordType : uint8_t {
    DATA_WORD = 0,
    COMMAND_WORD = 1
  };

  /**
    Structure representing a decoded ARINC 629 word.
  */
  struct Word {
    uint16_t data;    // 16 bits data
    WordType type;    // Derived from sync
    bool parity_ok;   // 1 bit parity (odd)
  };

private:
  Word last_word_;
  Fl_ARINC629_Callback arinc629_cb_;
  void* arinc629_user_data_;
  uint8_t buffer_[3]; // 3 bytes to fit 20-bits
  uint8_t buf_idx_;

  // Internal serial callback to feed the parser
  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);
  void decode_word();
  bool check_parity(uint32_t raw_20bit) const;

public:
  Fl_ARINC629();
  virtual ~Fl_ARINC629();

  /**
    Opens the serial port.
    \param port_name The path or name of the port.
    \return 0 on success, or -1 on error.
  */
  int open(const char* port_name);

  /**
    Registers a callback function to be called when a valid ARINC 629 word is received.
    \param cb The callback function.
    \param user_data Optional user data.
  */
  void arinc629_callback(Fl_ARINC629_Callback cb, void* user_data = nullptr);

  /**
    Gets the last received and decoded ARINC 629 word.
    \return The last decoded word structure.
  */
  const Word& last_word() const { return last_word_; }
  
  // For unit testing: manually feed a byte into the parser
  void feed_byte(uint8_t b) { process_byte(b); }
  
  // For unit testing: manually feed a 24-bit raw word
  void feed_raw_word(uint32_t w);
};

#endif
