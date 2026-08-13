//
// CoAP Client class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_CoAP_Client_H
#define Fl_CoAP_Client_H

#include "Fl_Export.H"
#include <string>
#include <FL/platform_types.h>

class Fl_CoAP_Client;

/**
  Callback type for CoAP response reception.
  \param client The CoAP client instance.
  \param message_id The message ID of the response.
  \param code The response code (e.g. 69 for 2.05 Content).
  \param payload Pointer to the response payload.
  \param payload_len Length of the response payload in bytes.
  \param userdata User data provided when setting the callback.
*/
typedef void (*Fl_CoAP_Response_Callback)(Fl_CoAP_Client* client, int message_id, int code, const void* payload, int payload_len, void* userdata);

/**
  Cross-platform CoAP Client.
  
  This class provides Qt-like (QCoapClient) capabilities for sending CoAP requests
  (GET, POST, PUT, DELETE) to a CoAP server and receiving responses.
  Network events are handled asynchronously via FLTK's Fl::add_fd().
*/
class FL_EXPORT Fl_CoAP_Client {
public:
  enum Method {
    Get = 1,
    Post = 2,
    Put = 3,
    Delete = 4
  };

private:
#if defined(_WIN32)
  uintptr_t socket_; // SOCKET on Windows
#else
  int socket_; // File descriptor on POSIX
#endif

  std::string hostname_;
  int port_;
  
  Fl_CoAP_Response_Callback response_cb_;
  void* response_cb_data_;
  
  // Buffer for incoming packets
  std::string recv_buffer_;
  
  // Internal socket handler for Fl::add_fd
  static void socket_cb_static(FL_SOCKET fd, void* data);
  void socket_cb();
  
  // CoAP Protocol helpers
  int send_packet(const void* data, int len);
  void handle_packet(const unsigned char* data, int len);

public:
  /**
    Initializes a new CoAP client.
  */
  Fl_CoAP_Client();
  
  /**
    Cleans up the CoAP client and closes any open connection.
  */
  ~Fl_CoAP_Client();

  // --- Configuration ---

  /** Sets the hostname or IP address of the CoAP server. */
  void set_hostname(const char* hostname);
  /** Returns the current hostname. */
  const char* hostname() const;

  /** Sets the port of the CoAP server (default is 5683). */
  void set_port(int port);
  /** Returns the current port. */
  int port() const;

  // --- Connection ---

  /**
    Initiates a connection to the CoAP server.
    This call blocks during socket creation and DNS resolution, but subsequent network IO is asynchronous.
    \return 0 on success, -1 on failure.
  */
  int connect();
  
  /**
    Disconnects from the CoAP server and closes the socket.
  */
  void disconnect();
  
  /** Returns true if the client has a valid open socket. */
  bool is_connected() const;

  // --- Messaging ---

  /**
    Sends a CoAP request to a specific path.
    \param method The CoAP method (Get, Post, Put, Delete).
    \param path The URI path to request.
    \param payload The payload data (can be NULL).
    \param payload_len The length of the payload data.
    \return the message ID (> 0) on success, or -1 if not connected.
  */
  int request(Method method, const char* path, const void* payload = NULL, int payload_len = 0);
  
  /**
    Sends a string CoAP request to a specific path.
    \param method The CoAP method (Get, Post, Put, Delete).
    \param path The URI path to request.
    \param payload The string payload.
    \return the message ID (> 0) on success, or -1 if not connected.
  */
  int request(Method method, const char* path, const char* payload);

  // --- Callbacks ---

  /** Sets the callback invoked when a CoAP response is received. */
  void set_response_callback(Fl_CoAP_Response_Callback cb, void* userdata = NULL);
};

#endif
