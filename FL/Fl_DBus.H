//
// Fl_DBus class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_DBus_H
#define Fl_DBus_H

#include "Fl_Export.H"
#include <string>

/**
  Cross-platform D-Bus wrapper.
  
  This class provides Qt-like capabilities for connecting to 
  System and Session buses and executing basic method calls.
*/
class FL_EXPORT Fl_DBus {
public:
  enum BusType {
    SessionBus,
    SystemBus
  };

private:
  void* connection_;
  BusType type_;
  bool is_connected_;

public:
  /**
    Initializes a D-Bus connection.
    \param type The bus type (SessionBus or SystemBus).
  */
  Fl_DBus(BusType type = SessionBus);
  
  /**
    Cleans up the connection.
  */
  ~Fl_DBus();

  /**
    Returns the bus type.
  */
  BusType type() const { return type_; }

  /**
    Returns whether the bus is currently connected.
  */
  bool is_connected() const;

  /**
    Performs a synchronous method call over D-Bus returning a string response.
    
    \param service The destination service name (e.g., "org.freedesktop.DBus").
    \param path The object path (e.g., "/org/freedesktop/DBus").
    \param interface The interface name (e.g., "org.freedesktop.DBus").
    \param method The method name (e.g., "GetMachineId").
    \param arg Optional string argument.
    \return The response string (or empty if none/failed).
  */
  std::string call_method(const char* service, 
                          const char* path, 
                          const char* interface, 
                          const char* method, 
                          const char* arg = 0);
};

#endif
