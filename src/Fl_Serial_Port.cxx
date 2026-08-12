//
// Serial Port class for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Serial_Port.H>
#include <FL/Fl.H>

#if defined(_WIN32)
#include <windows.h>
#include <thread>
#include <atomic>

struct WinThreadData {
  std::thread t;
  std::atomic<bool> running;
};

static void windows_awake_cb(void* data) {
  Fl_Serial_Port* port = (Fl_Serial_Port*)data;
  port->do_callback();
}

static void serial_listener_thread(Fl_Serial_Port* port, WinThreadData* td, HANDLE handle) {
  DWORD last_cbInQue = 0;
  while (td->running) {
    DWORD errors;
    COMSTAT comstat;
    if (ClearCommError(handle, &errors, &comstat)) {
      if (comstat.cbInQue > 0 && comstat.cbInQue > last_cbInQue) {
        if (port->callback()) {
          Fl::awake(windows_awake_cb, port);
        }
      }
      last_cbInQue = comstat.cbInQue;
    }
    Sleep(10);
  }
}

Fl_Serial_Port::Fl_Serial_Port() : callback_(nullptr), user_data_(nullptr), handle_(INVALID_HANDLE_VALUE), thread_data_(nullptr) {
}

void Fl_Serial_Port::callback(Fl_Serial_Callback cb, void* user_data) {
  callback_ = cb;
  user_data_ = user_data;
}

void Fl_Serial_Port::do_callback() {
  if (callback_) {
    callback_(this, user_data_);
  }
}

Fl_Serial_Port::~Fl_Serial_Port() {
  close();
}

int Fl_Serial_Port::open(const char* port_name) {
  if (is_open()) return -1;
  handle_ = CreateFileA(port_name, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                        OPEN_EXISTING, 0, NULL);
  if (handle_ == INVALID_HANDLE_VALUE) {
    return -1;
  }
  
  // Set default timeouts (non-blocking read)
  COMMTIMEOUTS timeouts;
  timeouts.ReadIntervalTimeout = MAXDWORD;
  timeouts.ReadTotalTimeoutMultiplier = 0;
  timeouts.ReadTotalTimeoutConstant = 0;
  timeouts.WriteTotalTimeoutMultiplier = 0;
  timeouts.WriteTotalTimeoutConstant = 0;
  SetCommTimeouts(handle_, &timeouts);
  
  WinThreadData* td = new WinThreadData();
  td->running = true;
  td->t = std::thread(serial_listener_thread, this, td, handle_);
  thread_data_ = td;
  
  return 0;
}

int Fl_Serial_Port::close() {
  if (!is_open()) return -1;
  
  if (thread_data_) {
    WinThreadData* td = (WinThreadData*)thread_data_;
    td->running = false;
    if (td->t.joinable()) {
      td->t.join();
    }
    delete td;
    thread_data_ = nullptr;
  }
  
  CloseHandle(handle_);
  handle_ = INVALID_HANDLE_VALUE;
  return 0;
}

int Fl_Serial_Port::is_open() const {
  return handle_ != INVALID_HANDLE_VALUE;
}

int Fl_Serial_Port::set_baud_rate(int baud) {
  if (!is_open()) return -1;
  DCB dcb;
  SecureZeroMemory(&dcb, sizeof(DCB));
  dcb.DCBlength = sizeof(DCB);
  if (!GetCommState(handle_, &dcb)) return -1;
  dcb.BaudRate = baud;
  if (!SetCommState(handle_, &dcb)) return -1;
  return 0;
}

int Fl_Serial_Port::set_data_bits(DataBits bits) {
  if (!is_open()) return -1;
  DCB dcb;
  SecureZeroMemory(&dcb, sizeof(DCB));
  dcb.DCBlength = sizeof(DCB);
  if (!GetCommState(handle_, &dcb)) return -1;
  dcb.ByteSize = (BYTE)bits;
  if (!SetCommState(handle_, &dcb)) return -1;
  return 0;
}

int Fl_Serial_Port::set_parity(Parity parity) {
  if (!is_open()) return -1;
  DCB dcb;
  SecureZeroMemory(&dcb, sizeof(DCB));
  dcb.DCBlength = sizeof(DCB);
  if (!GetCommState(handle_, &dcb)) return -1;
  dcb.fParity = (parity != PARITY_NONE) ? TRUE : FALSE;
  switch (parity) {
    case PARITY_NONE:  dcb.Parity = NOPARITY; break;
    case PARITY_ODD:   dcb.Parity = ODDPARITY; break;
    case PARITY_EVEN:  dcb.Parity = EVENPARITY; break;
    case PARITY_MARK:  dcb.Parity = MARKPARITY; break;
    case PARITY_SPACE: dcb.Parity = SPACEPARITY; break;
  }
  if (!SetCommState(handle_, &dcb)) return -1;
  return 0;
}

int Fl_Serial_Port::set_stop_bits(StopBits bits) {
  if (!is_open()) return -1;
  DCB dcb;
  SecureZeroMemory(&dcb, sizeof(DCB));
  dcb.DCBlength = sizeof(DCB);
  if (!GetCommState(handle_, &dcb)) return -1;
  switch (bits) {
    case STOP_1:   dcb.StopBits = ONESTOPBIT; break;
    case STOP_1_5: dcb.StopBits = ONE5STOPBITS; break;
    case STOP_2:   dcb.StopBits = TWOSTOPBITS; break;
  }
  if (!SetCommState(handle_, &dcb)) return -1;
  return 0;
}

int Fl_Serial_Port::write_data(const void* data, int len) {
  if (!is_open()) return -1;
  DWORD written = 0;
  if (!WriteFile(handle_, data, len, &written, NULL)) return -1;
  return (int)written;
}

int Fl_Serial_Port::read_data(void* buffer, int len) {
  if (!is_open()) return -1;
  DWORD read_bytes = 0;
  if (!ReadFile(handle_, buffer, len, &read_bytes, NULL)) return -1;
  return (int)read_bytes;
}

#else // POSIX (Linux, macOS, etc.)

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <FL/Fl.H>

Fl_Serial_Port::Fl_Serial_Port() : callback_(nullptr), user_data_(nullptr), fd_(-1) {
}

void Fl_Serial_Port::callback(Fl_Serial_Callback cb, void* user_data) {
  callback_ = cb;
  user_data_ = user_data;
  if (is_open()) {
    if (cb) Fl::add_fd(fd_, FL_READ, [](int, void* data) {
      ((Fl_Serial_Port*)data)->do_callback();
    }, this);
    else Fl::remove_fd(fd_);
  }
}

void Fl_Serial_Port::do_callback() {
  if (callback_) {
    callback_(this, user_data_);
  }
}

Fl_Serial_Port::~Fl_Serial_Port() {
  close();
}

int Fl_Serial_Port::open(const char* port_name) {
  if (is_open()) return -1;
  fd_ = ::open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd_ == -1) {
    return -1;
  }
  fcntl(fd_, F_SETFL, 0); // Clear O_NDELAY to enable blocking reads if configured later
  
  if (callback_) {
    Fl::add_fd(fd_, FL_READ, [](int, void* data) {
      ((Fl_Serial_Port*)data)->do_callback();
    }, this);
  }
  
  return 0;
}

int Fl_Serial_Port::close() {
  if (!is_open()) return -1;
  if (callback_) {
    Fl::remove_fd(fd_);
  }
  ::close(fd_);
  fd_ = -1;
  return 0;
}

int Fl_Serial_Port::is_open() const {
  return fd_ != -1;
}

int Fl_Serial_Port::set_baud_rate(int baud) {
  if (!is_open()) return -1;
  struct termios tty;
  if (tcgetattr(fd_, &tty) != 0) return -1;

  speed_t speed;
  switch (baud) {
#ifdef B50
    case 50: speed = B50; break;
#endif
#ifdef B75
    case 75: speed = B75; break;
#endif
#ifdef B110
    case 110: speed = B110; break;
#endif
#ifdef B134
    case 134: speed = B134; break;
#endif
#ifdef B150
    case 150: speed = B150; break;
#endif
#ifdef B200
    case 200: speed = B200; break;
#endif
#ifdef B300
    case 300: speed = B300; break;
#endif
#ifdef B600
    case 600: speed = B600; break;
#endif
#ifdef B1200
    case 1200: speed = B1200; break;
#endif
#ifdef B1800
    case 1800: speed = B1800; break;
#endif
#ifdef B2400
    case 2400: speed = B2400; break;
#endif
#ifdef B4800
    case 4800: speed = B4800; break;
#endif
    case 9600: speed = B9600; break;
#ifdef B19200
    case 19200: speed = B19200; break;
#endif
#ifdef B38400
    case 38400: speed = B38400; break;
#endif
#ifdef B57600
    case 57600: speed = B57600; break;
#endif
#ifdef B115200
    case 115200: speed = B115200; break;
#endif
#ifdef B230400
    case 230400: speed = B230400; break;
#endif
#ifdef B460800
    case 460800: speed = B460800; break;
#endif
#ifdef B500000
    case 500000: speed = B500000; break;
#endif
#ifdef B576000
    case 576000: speed = B576000; break;
#endif
#ifdef B921600
    case 921600: speed = B921600; break;
#endif
#ifdef B1000000
    case 1000000: speed = B1000000; break;
#endif
#ifdef B1152000
    case 1152000: speed = B1152000; break;
#endif
#ifdef B1500000
    case 1500000: speed = B1500000; break;
#endif
#ifdef B2000000
    case 2000000: speed = B2000000; break;
#endif
#ifdef B2500000
    case 2500000: speed = B2500000; break;
#endif
#ifdef B3000000
    case 3000000: speed = B3000000; break;
#endif
#ifdef B3500000
    case 3500000: speed = B3500000; break;
#endif
#ifdef B4000000
    case 4000000: speed = B4000000; break;
#endif
    default: return -1; // Unsupported baud rate
  }
  
  cfsetospeed(&tty, speed);
  cfsetispeed(&tty, speed);
  if (tcsetattr(fd_, TCSANOW, &tty) != 0) return -1;
  return 0;
}

int Fl_Serial_Port::set_data_bits(DataBits bits) {
  if (!is_open()) return -1;
  struct termios tty;
  if (tcgetattr(fd_, &tty) != 0) return -1;
  
  tty.c_cflag &= ~CSIZE;
  switch (bits) {
    case DATA_5: tty.c_cflag |= CS5; break;
    case DATA_6: tty.c_cflag |= CS6; break;
    case DATA_7: tty.c_cflag |= CS7; break;
    case DATA_8: tty.c_cflag |= CS8; break;
  }
  
  if (tcsetattr(fd_, TCSANOW, &tty) != 0) return -1;
  return 0;
}

int Fl_Serial_Port::set_parity(Parity parity) {
  if (!is_open()) return -1;
  struct termios tty;
  if (tcgetattr(fd_, &tty) != 0) return -1;
  
  tty.c_cflag &= ~(PARENB | PARODD);
#ifdef CMSPAR
  tty.c_cflag &= ~CMSPAR;
#endif
  
  switch (parity) {
    case PARITY_NONE: 
      break;
    case PARITY_ODD:
      tty.c_cflag |= PARENB | PARODD;
      break;
    case PARITY_EVEN:
      tty.c_cflag |= PARENB;
      break;
    case PARITY_MARK:
#ifdef CMSPAR
      tty.c_cflag |= PARENB | PARODD | CMSPAR;
#else
      return -1; // Not supported on this platform
#endif
      break;
    case PARITY_SPACE:
#ifdef CMSPAR
      tty.c_cflag |= PARENB | CMSPAR;
#else
      return -1; // Not supported on this platform
#endif
      break;
  }
  
  if (tcsetattr(fd_, TCSANOW, &tty) != 0) return -1;
  return 0;
}

int Fl_Serial_Port::set_stop_bits(StopBits bits) {
  if (!is_open()) return -1;
  struct termios tty;
  if (tcgetattr(fd_, &tty) != 0) return -1;
  
  switch (bits) {
    case STOP_1:
      tty.c_cflag &= ~CSTOPB;
      break;
    case STOP_1_5:
    case STOP_2:
      tty.c_cflag |= CSTOPB;
      break;
  }
  
  if (tcsetattr(fd_, TCSANOW, &tty) != 0) return -1;
  return 0;
}

int Fl_Serial_Port::write_data(const void* data, int len) {
  if (!is_open()) return -1;
  return (int)::write(fd_, data, len);
}

int Fl_Serial_Port::read_data(void* buffer, int len) {
  if (!is_open()) return -1;
  return (int)::read(fd_, buffer, len);
}

#endif
