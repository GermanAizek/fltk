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

#include <FL/Fl_MQTT_Client.H>
#include <FL/Fl.H>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

// --- MQTT 3.1.1 Packet Types ---
enum MQTTPacketType {
    CONNECT     = 1 << 4,
    CONNACK     = 2 << 4,
    PUBLISH     = 3 << 4,
    PUBACK      = 4 << 4,
    SUBSCRIBE   = 8 << 4,
    SUBACK      = 9 << 4,
    UNSUBSCRIBE = 10 << 4,
    UNSUBACK    = 11 << 4,
    PINGREQ     = 12 << 4,
    PINGRESP    = 13 << 4,
    DISCONNECT  = 14 << 4
};

// --- Helper Functions for MQTT Encoding ---
static void append_string(std::string& buf, const std::string& str) {
    int len = (int)str.length();
    buf.push_back((char)(len >> 8));
    buf.push_back((char)(len & 0xFF));
    buf.append(str);
}

static void encode_remaining_length(std::string& buf, int len) {
    do {
        char encodedByte = len % 128;
        len = len / 128;
        if (len > 0) {
            encodedByte |= 128;
        }
        buf.push_back(encodedByte);
    } while (len > 0);
}

Fl_MQTT_Client::Fl_MQTT_Client() : 
#if defined(_WIN32)
    socket_((uintptr_t)-1),
#else
    socket_(-1),
#endif
    hostname_("localhost"),
    port_(1883),
    client_id_("FLTK_MQTT_Client"),
    keep_alive_(60),
    state_(Disconnected),
    message_cb_(NULL),
    message_cb_data_(NULL),
    connected_cb_(NULL),
    connected_cb_data_(NULL),
    disconnected_cb_(NULL),
    disconnected_cb_data_(NULL)
{
#if defined(_WIN32)
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

Fl_MQTT_Client::~Fl_MQTT_Client() {
    disconnect();
#if defined(_WIN32)
    WSACleanup();
#endif
}

void Fl_MQTT_Client::set_hostname(const char* hostname) { hostname_ = hostname ? hostname : ""; }
const char* Fl_MQTT_Client::hostname() const { return hostname_.c_str(); }

void Fl_MQTT_Client::set_port(int port) { port_ = port; }
int Fl_MQTT_Client::port() const { return port_; }

void Fl_MQTT_Client::set_client_id(const char* client_id) { client_id_ = client_id ? client_id : ""; }
const char* Fl_MQTT_Client::client_id() const { return client_id_.c_str(); }

void Fl_MQTT_Client::set_username(const char* username) { username_ = username ? username : ""; }
void Fl_MQTT_Client::set_password(const char* password) { password_ = password ? password : ""; }

void Fl_MQTT_Client::set_keep_alive(int keep_alive) { keep_alive_ = keep_alive; }

Fl_MQTT_Client::ClientState Fl_MQTT_Client::state() const { return state_; }

int Fl_MQTT_Client::connect() {
    if (state_ != Disconnected) {
        return -1;
    }

    // 1. Resolve host and connect socket
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port_);
    
    if (getaddrinfo(hostname_.c_str(), port_str, &hints, &res) != 0) {
        return -1;
    }

#if defined(_WIN32)
    SOCKET s = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (s == INVALID_SOCKET) {
        freeaddrinfo(res);
        return -1;
    }
#else
    int s = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (s < 0) {
        freeaddrinfo(res);
        return -1;
    }
#endif

    if (::connect(s, res->ai_addr, (int)res->ai_addrlen) < 0) {
#if defined(_WIN32)
        ::closesocket(s);
#else
        ::close(s);
#endif
        freeaddrinfo(res);
        return -1;
    }
    freeaddrinfo(res);

    socket_ = (uintptr_t)s;
    state_ = Connecting;

    // 2. Set socket to non-blocking
#if defined(_WIN32)
    u_long mode = 1;
    ioctlsocket((SOCKET)socket_, FIONBIO, &mode);
#else
    int flags = fcntl((int)socket_, F_GETFL, 0);
    fcntl((int)socket_, F_SETFL, flags | O_NONBLOCK);
#endif

    // 3. Register FLTK fd callback
#if defined(_WIN32)
    Fl::add_fd((int)socket_, FL_READ | FL_EXCEPT, socket_cb_static, this);
#else
    Fl::add_fd((int)socket_, FL_READ | FL_EXCEPT, socket_cb_static, this);
#endif

    // 4. Send MQTT CONNECT packet
    std::string payload;
    append_string(payload, client_id_);
    if (!username_.empty()) {
        append_string(payload, username_);
    }
    if (!password_.empty()) {
        append_string(payload, password_);
    }

    std::string variable_header;
    append_string(variable_header, "MQTT");
    variable_header.push_back(4); // Protocol level (MQTT 3.1.1)
    
    char connect_flags = 0x02; // Clean session
    if (!username_.empty()) connect_flags |= 0x80;
    if (!password_.empty()) connect_flags |= 0x40;
    variable_header.push_back(connect_flags);
    
    variable_header.push_back((char)(keep_alive_ >> 8));
    variable_header.push_back((char)(keep_alive_ & 0xFF));

    std::string packet;
    packet.push_back(CONNECT);
    encode_remaining_length(packet, (int)(variable_header.length() + payload.length()));
    packet.append(variable_header);
    packet.append(payload);

    if (send_packet(packet.data(), (int)packet.length()) < 0) {
        disconnect();
        return -1;
    }

    return 0;
}

void Fl_MQTT_Client::disconnect() {
    if (state_ == Disconnected) return;

    if (state_ == Connected) {
        char packet[2] = { (char)DISCONNECT, 0 };
        send_packet(packet, 2);
    }

    if (socket_ != (uintptr_t)-1) {
#if defined(_WIN32)
        Fl::remove_fd((SOCKET)socket_);
        ::closesocket((SOCKET)socket_);
#else
        Fl::remove_fd((int)socket_);
        ::close((int)socket_);
#endif
#if defined(_WIN32)
        socket_ = (uintptr_t)-1;
#else
        socket_ = -1;
#endif
    }

    state_ = Disconnected;
    recv_buffer_.clear();

    if (disconnected_cb_) {
        disconnected_cb_(this, disconnected_cb_data_);
    }
}

int Fl_MQTT_Client::publish(const char* topic, const void* message, int len) {
    if (state_ != Connected) return -1;

    std::string variable_header;
    append_string(variable_header, std::string(topic));

    std::string packet;
    packet.push_back(PUBLISH);
    encode_remaining_length(packet, (int)(variable_header.length() + len));
    packet.append(variable_header);
    packet.append((const char*)message, len);

    return send_packet(packet.data(), (int)packet.length());
}

int Fl_MQTT_Client::publish(const char* topic, const char* message) {
    return publish(topic, message, (int)strlen(message));
}

int Fl_MQTT_Client::subscribe(const char* topic) {
    if (state_ != Connected) return -1;

    static unsigned short packet_id = 1;

    std::string variable_header;
    variable_header.push_back((char)(packet_id >> 8));
    variable_header.push_back((char)(packet_id & 0xFF));
    packet_id++;

    std::string payload;
    append_string(payload, std::string(topic));
    payload.push_back(0); // QoS 0

    std::string packet;
    packet.push_back(static_cast<char>(SUBSCRIBE | 0x02));
    encode_remaining_length(packet, (int)(variable_header.length() + payload.length()));
    packet.append(variable_header);
    packet.append(payload);

    return send_packet(packet.data(), (int)packet.length());
}

int Fl_MQTT_Client::unsubscribe(const char* topic) {
    if (state_ != Connected) return -1;

    static unsigned short packet_id = 1;

    std::string variable_header;
    variable_header.push_back((char)(packet_id >> 8));
    variable_header.push_back((char)(packet_id & 0xFF));
    packet_id++;

    std::string payload;
    append_string(payload, std::string(topic));

    std::string packet;
    packet.push_back(static_cast<char>(UNSUBSCRIBE | 0x02));
    encode_remaining_length(packet, (int)(variable_header.length() + payload.length()));
    packet.append(variable_header);
    packet.append(payload);

    return send_packet(packet.data(), (int)packet.length());
}

int Fl_MQTT_Client::send_packet(const void* data, int len) {
    if (socket_ == (uintptr_t)-1) return -1;
#if defined(_WIN32)
    int n = ::send((SOCKET)socket_, (const char*)data, len, 0);
#else
    int n = ::send((int)socket_, data, len, 0);
#endif
    return n == len ? 0 : -1;
}

void Fl_MQTT_Client::socket_cb_static(FL_SOCKET fd, void* data) {
    (void)fd;
    ((Fl_MQTT_Client*)data)->socket_cb();
}

void Fl_MQTT_Client::socket_cb() {
    char buf[1024];
#if defined(_WIN32)
    int n = ::recv((SOCKET)socket_, buf, sizeof(buf), 0);
#else
    int n = ::recv((int)socket_, buf, sizeof(buf), 0);
#endif

    if (n <= 0) {
        // Error or connection closed
        disconnect();
        return;
    }

    recv_buffer_.append(buf, n);

    // Parse packets
    while (recv_buffer_.length() > 1) {
        const unsigned char* data = (const unsigned char*)recv_buffer_.data();
        int multiplier = 1;
        int value = 0;
        size_t idx = 1;
        
        bool length_decoded = false;
        while (idx < recv_buffer_.length() && idx <= 4) {
            unsigned char encodedByte = data[idx++];
            value += (encodedByte & 127) * multiplier;
            multiplier *= 128;
            if ((encodedByte & 128) == 0) {
                length_decoded = true;
                break;
            }
        }

        if (!length_decoded) {
            break; // Need more data for length
        }

        int total_packet_len = (int)idx + value;
        if ((int)recv_buffer_.length() < total_packet_len) {
            break; // Need more data for body
        }

        handle_packet(data, total_packet_len);
        
        recv_buffer_.erase(0, total_packet_len);
    }
}

void Fl_MQTT_Client::handle_packet(const unsigned char* data, int len) {
    if (len < 2) return;
    
    unsigned char type = data[0] & 0xF0;
    
    // Find where the variable header starts
    size_t idx = 1;
    while (idx < (size_t)len && idx <= 4) {
        unsigned char b = data[idx++];
        if ((b & 128) == 0) break;
    }
    
    if (type == CONNACK) {
        if (len - idx >= 2) {
            unsigned char return_code = data[idx + 1];
            if (return_code == 0) {
                state_ = Connected;
                if (connected_cb_) connected_cb_(this, connected_cb_data_);
            } else {
                disconnect();
            }
        }
    } else if (type == PUBLISH) {
        if (len - idx >= 2) {
            int topic_len = (data[idx] << 8) | data[idx+1];
            idx += 2;
            if (len - idx >= (size_t)topic_len) {
                std::string topic((const char*)&data[idx], topic_len);
                idx += topic_len;
                
                // For QoS > 0, there would be a packet ID, but we only support QoS 0 for simplicity.
                // Note: If QoS is 1 or 2 (from data[0]), we should skip packet ID.
                unsigned char qos = (data[0] & 0x06) >> 1;
                if (qos > 0 && len - idx >= 2) {
                    idx += 2; // skip packet id
                }
                
                int payload_len = len - (int)idx;
                if (payload_len >= 0 && message_cb_) {
                    message_cb_(this, topic.c_str(), &data[idx], payload_len, message_cb_data_);
                }
            }
        }
    } else if (type == PINGREQ) {
        char packet[2] = { (char)PINGRESP, 0 };
        send_packet(packet, 2);
    }
}

void Fl_MQTT_Client::set_message_callback(Fl_MQTT_Message_Callback cb, void* userdata) {
    message_cb_ = cb;
    message_cb_data_ = userdata;
}

void Fl_MQTT_Client::set_connected_callback(Fl_MQTT_State_Callback cb, void* userdata) {
    connected_cb_ = cb;
    connected_cb_data_ = userdata;
}

void Fl_MQTT_Client::set_disconnected_callback(Fl_MQTT_State_Callback cb, void* userdata) {
    disconnected_cb_ = cb;
    disconnected_cb_data_ = userdata;
}
