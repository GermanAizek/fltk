//
// MQTT Client class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_MQTT_Client_H
#define Fl_MQTT_Client_H

#include "Fl_Export.H"
#include <string>

class Fl_MQTT_Client;

/**
  Callback type for MQTT message reception.
  \param client The MQTT client instance.
  \param topic The topic the message was published to.
  \param payload Pointer to the message payload.
  \param payload_len Length of the message payload in bytes.
  \param userdata User data provided when setting the callback.
*/
typedef void (*Fl_MQTT_Message_Callback)(Fl_MQTT_Client* client, const char* topic, const void* payload, int payload_len, void* userdata);

/**
  Callback type for MQTT state changes (connected, disconnected).
  \param client The MQTT client instance.
  \param userdata User data provided when setting the callback.
*/
typedef void (*Fl_MQTT_State_Callback)(Fl_MQTT_Client* client, void* userdata);

/**
  Cross-platform MQTT Client.
  
  This class provides Qt-like (QMqttClient) capabilities for connecting to an MQTT broker,
  publishing messages, and subscribing to topics using the MQTT 3.1.1 protocol.
  Network events are handled asynchronously via FLTK's Fl::add_fd().
*/
class FL_EXPORT Fl_MQTT_Client {
public:
  enum ClientState {
    Disconnected,
    Connecting,
    Connected
  };

private:
#if defined(_WIN32)
  uintptr_t socket_; // SOCKET on Windows
#else
  int socket_; // File descriptor on POSIX
#endif

  std::string hostname_;
  int port_;
  std::string client_id_;
  std::string username_;
  std::string password_;
  int keep_alive_;
  
  ClientState state_;
  
  Fl_MQTT_Message_Callback message_cb_;
  void* message_cb_data_;
  
  Fl_MQTT_State_Callback connected_cb_;
  void* connected_cb_data_;
  
  Fl_MQTT_State_Callback disconnected_cb_;
  void* disconnected_cb_data_;

  // Buffer for incoming packets
  std::string recv_buffer_;
  
  // Internal socket handler for Fl::add_fd
  static void socket_cb_static(int fd, void* data);
  void socket_cb();
  
  // MQTT Protocol helpers
  int send_packet(const void* data, int len);
  void handle_packet(const unsigned char* data, int len);

public:
  /**
    Initializes a new MQTT client.
  */
  Fl_MQTT_Client();
  
  /**
    Cleans up the MQTT client and closes any open connection.
  */
  ~Fl_MQTT_Client();

  // --- Configuration ---

  /** Sets the hostname or IP address of the MQTT broker. */
  void set_hostname(const char* hostname);
  /** Returns the current hostname. */
  const char* hostname() const;

  /** Sets the port of the MQTT broker (default is 1883). */
  void set_port(int port);
  /** Returns the current port. */
  int port() const;

  /** Sets the MQTT client ID. */
  void set_client_id(const char* client_id);
  /** Returns the current client ID. */
  const char* client_id() const;

  /** Sets the username for authentication. */
  void set_username(const char* username);
  /** Sets the password for authentication. */
  void set_password(const char* password);
  
  /** Sets the keep-alive interval in seconds (default is 60). */
  void set_keep_alive(int keep_alive);

  // --- Connection ---

  /**
    Initiates a connection to the MQTT broker.
    This call blocks during socket connection, but subsequent network IO is asynchronous.
    \return 0 on success, -1 on failure.
  */
  int connect();
  
  /**
    Disconnects from the MQTT broker gracefully.
  */
  void disconnect();
  
  /** Returns the current state of the client. */
  ClientState state() const;

  // --- Messaging ---

  /**
    Publishes a message to a specific topic with QoS 0.
    \param topic The topic to publish to.
    \param message The payload data.
    \param len The length of the payload data.
    \return 0 on success, -1 if not connected.
  */
  int publish(const char* topic, const void* message, int len);
  
  /**
    Publishes a string message to a specific topic with QoS 0.
    \param topic The topic to publish to.
    \param message The string payload.
    \return 0 on success, -1 if not connected.
  */
  int publish(const char* topic, const char* message);

  /**
    Subscribes to a specific topic filter with QoS 0.
    \param topic The topic filter to subscribe to.
    \return 0 on success, -1 if not connected.
  */
  int subscribe(const char* topic);
  
  /**
    Unsubscribes from a specific topic filter.
    \param topic The topic filter to unsubscribe from.
    \return 0 on success, -1 if not connected.
  */
  int unsubscribe(const char* topic);

  // --- Callbacks ---

  /** Sets the callback invoked when an MQTT message is received. */
  void set_message_callback(Fl_MQTT_Message_Callback cb, void* userdata = NULL);
  
  /** Sets the callback invoked when the client successfully connects. */
  void set_connected_callback(Fl_MQTT_State_Callback cb, void* userdata = NULL);
  
  /** Sets the callback invoked when the client disconnects. */
  void set_disconnected_callback(Fl_MQTT_State_Callback cb, void* userdata = NULL);
};

#endif
