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

#include <FL/Fl_CoAP_Client.H>
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

// --- CoAP Option Numbers ---
enum CoAPOptionNumber {
    COAP_OPTION_URI_PATH = 11,
};

Fl_CoAP_Client::Fl_CoAP_Client() : 
#if defined(_WIN32)
    socket_((uintptr_t)-1),
#else
    socket_(-1),
#endif
    hostname_("localhost"),
    port_(5683), // Default CoAP port
    response_cb_(NULL),
    response_cb_data_(NULL)
{
#if defined(_WIN32)
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

Fl_CoAP_Client::~Fl_CoAP_Client() {
    disconnect();
#if defined(_WIN32)
    WSACleanup();
#endif
}

void Fl_CoAP_Client::set_hostname(const char* hostname) { hostname_ = hostname ? hostname : ""; }
const char* Fl_CoAP_Client::hostname() const { return hostname_.c_str(); }

void Fl_CoAP_Client::set_port(int port) { port_ = port; }
int Fl_CoAP_Client::port() const { return port_; }

bool Fl_CoAP_Client::is_connected() const {
#if defined(_WIN32)
    return socket_ != (uintptr_t)-1;
#else
    return socket_ != -1;
#endif
}

int Fl_CoAP_Client::connect() {
    if (is_connected()) {
        return -1;
    }

    // 1. Resolve host and connect UDP socket
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    
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

    // For UDP, connect() just sets the default destination address
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

    return 0;
}

void Fl_CoAP_Client::disconnect() {
    if (!is_connected()) return;

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

    recv_buffer_.clear();
}

int Fl_CoAP_Client::request(Method method, const char* path, const void* payload, int payload_len) {
    if (!is_connected()) return -1;

    static unsigned short message_id = 1;

    std::string packet;
    
    // Header (4 bytes)
    // Version (2) = 1, Type (2) = 0 (CON), Token Length (4) = 4
    packet.push_back((1 << 6) | (0 << 4) | 4);
    packet.push_back((char)method);
    packet.push_back((char)(message_id >> 8));
    packet.push_back((char)(message_id & 0xFF));
    
    // Token (4 bytes)
    unsigned int token = (unsigned int)message_id;
    packet.push_back((char)((token >> 24) & 0xFF));
    packet.push_back((char)((token >> 16) & 0xFF));
    packet.push_back((char)((token >> 8) & 0xFF));
    packet.push_back((char)(token & 0xFF));

    // Options
    // URI-Path option (11)
    if (path && strlen(path) > 0) {
        std::string p(path);
        if (p[0] == '/') p.erase(0, 1);
        
        size_t pos = 0;
        int last_opt = 0;
        while ((pos = p.find('/')) != std::string::npos || p.length() > 0) {
            std::string segment;
            if (pos != std::string::npos) {
                segment = p.substr(0, pos);
                p.erase(0, pos + 1);
            } else {
                segment = p;
                p.clear();
            }
            
            if (segment.empty()) continue;
            
            int opt_delta = COAP_OPTION_URI_PATH - last_opt;
            int opt_len = (int)segment.length();
            
            // Simplified option header (assuming delta < 13 and len < 13 for basic URIs)
            unsigned char opt_head = 0;
            if (opt_delta < 13) opt_head |= (opt_delta << 4);
            // else not implemented for this basic client
            
            if (opt_len < 13) opt_head |= opt_len;
            // else not implemented for this basic client
            
            packet.push_back((char)opt_head);
            packet.append(segment);
            
            last_opt = COAP_OPTION_URI_PATH;
        }
    }

    // Payload
    if (payload && payload_len > 0) {
        packet.push_back((char)0xFF); // Payload marker
        packet.append((const char*)payload, payload_len);
    }

    if (send_packet(packet.data(), (int)packet.length()) < 0) {
        return -1;
    }

    int ret_msg_id = message_id;
    message_id++;
    return ret_msg_id;
}

int Fl_CoAP_Client::request(Method method, const char* path, const char* payload) {
    return request(method, path, payload, payload ? (int)strlen(payload) : 0);
}

int Fl_CoAP_Client::send_packet(const void* data, int len) {
    if (!is_connected()) return -1;
#if defined(_WIN32)
    int n = ::send((SOCKET)socket_, (const char*)data, len, 0);
#else
    int n = ::send((int)socket_, data, len, 0);
#endif
    return n == len ? 0 : -1;
}

void Fl_CoAP_Client::socket_cb_static(FL_SOCKET fd, void* data) {
    (void)fd;
    ((Fl_CoAP_Client*)data)->socket_cb();
}

void Fl_CoAP_Client::socket_cb() {
    char buf[2048];
#if defined(_WIN32)
    int n = ::recv((SOCKET)socket_, buf, sizeof(buf), 0);
#else
    int n = ::recv((int)socket_, buf, sizeof(buf), 0);
#endif

    if (n <= 0) {
        return;
    }
    
    handle_packet((const unsigned char*)buf, n);
}

void Fl_CoAP_Client::handle_packet(const unsigned char* data, int len) {
    if (len < 4) return;
    
    int version = (data[0] >> 6) & 0x03;
    if (version != 1) return;
    
    int type = (data[0] >> 4) & 0x03;
    (void)type;
    int tkl = data[0] & 0x0F;
    int code = data[1];
    int message_id = (data[2] << 8) | data[3];
    
    if (len < 4 + tkl) return;
    
    int idx = 4 + tkl;
    
    // Skip options
    while (idx < len && data[idx] != 0xFF) {
        int opt_delta = (data[idx] >> 4) & 0x0F;
        int opt_len = data[idx] & 0x0F;
        idx++;
        
        if (opt_delta == 13) {
            if (idx >= len) return;
            opt_delta = data[idx++] + 13;
        } else if (opt_delta == 14) {
            if (idx + 1 >= len) return;
            opt_delta = (data[idx] << 8) | data[idx+1];
            opt_delta += 269;
            idx += 2;
        }
        
        if (opt_len == 13) {
            if (idx >= len) return;
            opt_len = data[idx++] + 13;
        } else if (opt_len == 14) {
            if (idx + 1 >= len) return;
            opt_len = (data[idx] << 8) | data[idx+1];
            opt_len += 269;
            idx += 2;
        }
        
        idx += opt_len;
    }
    
    int payload_len = 0;
    const unsigned char* payload = NULL;
    
    if (idx < len && data[idx] == 0xFF) {
        idx++;
        payload = &data[idx];
        payload_len = len - idx;
    }
    
    if (response_cb_) {
        // Only trigger callback for Responses (ACK=2, or CON/NON with response code)
        // Code class 2 (Success), 4 (Client Error), 5 (Server Error)
        int code_class = code >> 5;
        if (code_class >= 2 && code_class <= 5) {
            response_cb_(this, message_id, code, payload, payload_len, response_cb_data_);
        }
    }
}

void Fl_CoAP_Client::set_response_callback(Fl_CoAP_Response_Callback cb, void* userdata) {
    response_cb_ = cb;
    response_cb_data_ = userdata;
}
