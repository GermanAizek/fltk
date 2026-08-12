//
// Bluetooth class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Bluetooth_H
#define Fl_Bluetooth_H

#include "Fl_Export.H"
#include <string>
#include <vector>

/**
  Represents a discovered Bluetooth device.
*/
struct FL_EXPORT Fl_Bluetooth_Device {
  std::string address;
  std::string name;
};

/**
  Cross-platform Bluetooth wrapper.
  
  This class provides Qt-like capabilities for local device management, 
  device discovery, and RFCOMM socket connection.
*/
class FL_EXPORT Fl_Bluetooth {
public:
  enum HostMode {
    HostPoweredOff,
    HostConnectable,
    HostDiscoverable,
    HostDiscoverableAndConnectable
  };

private:
#if defined(_WIN32)
  uintptr_t socket_; // SOCKET on Windows
#else
  int socket_; // File descriptor on POSIX
#endif

public:
  /**
    Initializes a Bluetooth socket instance.
  */
  Fl_Bluetooth();
  
  /**
    Cleans up the Bluetooth socket, closing it if open.
  */
  ~Fl_Bluetooth();

  // --- Local Device (equivalent to QBluetoothLocalDevice) ---
  
  /**
    Returns the MAC address of the local Bluetooth adapter.
  */
  static std::string local_address();
  
  /**
    Returns the name of the local Bluetooth adapter.
  */
  static std::string local_name();
  
  /**
    Powers on the local Bluetooth adapter.
  */
  static void power_on();
  
  /**
    Sets the host mode of the local Bluetooth adapter.
  */
  static void set_host_mode(HostMode mode);
  
  /**
    Returns the current host mode.
  */
  static HostMode host_mode();

  // --- Discovery (equivalent to QBluetoothDeviceDiscoveryAgent) ---
  
  /**
    Scans for nearby Bluetooth devices.
    \param timeout_ms Timeout in milliseconds.
    \return A vector of discovered devices.
  */
  static std::vector<Fl_Bluetooth_Device> scan_devices(int timeout_ms = 5000);

  // --- Socket (equivalent to QBluetoothSocket) ---
  
  /**
    Connects to a remote Bluetooth device.
    \param address The MAC address of the remote device (e.g. "00:11:22:33:44:55").
    \param channel The RFCOMM channel to connect to.
    \return 0 on success, or -1 on error.
  */
  int connect(const char* address, int channel = 1);
  
  /**
    Closes the current socket connection.
    \return 0 on success, or -1 if not connected.
  */
  int close();

  /**
    Checks if the socket is connected.
    \return 1 if connected, 0 if not.
  */
  int is_open() const;

  /**
    Writes multiple bytes to the Bluetooth socket.
    \param data Pointer to the buffer containing data.
    \param len Number of bytes to write.
    \return The number of bytes written, or -1 on error.
  */
  int write_data(const void* data, int len);
  
  /**
    Reads multiple bytes from the Bluetooth socket.
    \param buffer Pointer to the buffer to store read data.
    \param len Maximum number of bytes to read.
    \return The number of bytes read, or -1 on error.
  */
  int read_data(void* buffer, int len);
};

#endif
