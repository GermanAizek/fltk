//
// FLTK NFC (Near Field Communication) interface
//
// Copyright 2026 by GermanAizek.
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

#ifndef Fl_NFC_H
#define Fl_NFC_H

#include <FL/Fl_Export.H>
#include <string>
#include <vector>

class Fl_NFC_Driver;

/**
  Represents a single NDEF record on an NFC tag.
*/
class FL_EXPORT Fl_NFC_Record {
  std::string type_;
  std::string payload_;
public:
  Fl_NFC_Record(const std::string& type, const std::string& payload);
  const std::string& type() const { return type_; }
  const std::string& payload() const { return payload_; }
};

/**
  Represents a collection of NDEF records on an NFC tag.
*/
class FL_EXPORT Fl_NFC_Message {
  std::vector<Fl_NFC_Record> records_;
public:
  Fl_NFC_Message() = default;
  void add_record(const Fl_NFC_Record& record) { records_.push_back(record); }
  const std::vector<Fl_NFC_Record>& records() const { return records_; }
};

/**
  Represents an NFC target (tag or device) detected by the NFC manager.
*/
class FL_EXPORT Fl_NFC_Target {
  Fl_NFC_Message message_;
public:
  Fl_NFC_Target() {}
  virtual ~Fl_NFC_Target() {}
  
  void message(const Fl_NFC_Message& msg) { message_ = msg; }
  const Fl_NFC_Message& message() const { return message_; }
};

/**
  Callback type for target detection.
*/
typedef void (*Fl_NFC_Target_Callback)(Fl_NFC_Target* target, void* data);

/**
  The Fl_NFC_Manager class provides an interface to access NFC hardware.
  
  This is a cross-platform wrapper around OS-specific NFC implementations.
*/
class FL_EXPORT Fl_NFC_Manager {
  Fl_NFC_Driver* driver_;
  Fl_NFC_Target_Callback target_detected_cb_;
  Fl_NFC_Target_Callback target_lost_cb_;
  void* user_data_;

public:
  Fl_NFC_Manager();
  ~Fl_NFC_Manager();

  /**
    Start target detection.
    Returns 1 on success, 0 on failure.
  */
  int start_target_detection();

  /**
    Stop target detection.
  */
  void stop_target_detection();

  /**
    Set the callback for when an NFC target is detected.
  */
  void target_detected_callback(Fl_NFC_Target_Callback cb, void* data = 0);
  
  /**
    Set the callback for when an NFC target is lost.
  */
  void target_lost_callback(Fl_NFC_Target_Callback cb, void* data = 0);

  /** Internal use only. */
  void handle_target_detected(Fl_NFC_Target* target);
  /** Internal use only. */
  void handle_target_lost(Fl_NFC_Target* target);
};

#endif // Fl_NFC_H
