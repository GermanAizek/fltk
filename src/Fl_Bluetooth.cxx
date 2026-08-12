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

#include <FL/Fl_Bluetooth.H>

#if defined(_WIN32)
#include <windows.h>
#include <winsock2.h>
#include <ws2bth.h>
#include <bluetoothapis.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "bthprops.lib")

Fl_Bluetooth::Fl_Bluetooth() : socket_((uintptr_t)INVALID_SOCKET) {
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
}

Fl_Bluetooth::~Fl_Bluetooth() {
  close();
  WSACleanup();
}

std::string Fl_Bluetooth::local_address() {
  HANDLE hRadio;
  BLUETOOTH_FIND_RADIO_PARAMS params = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };
  HBLUETOOTH_RADIO_FIND hFind = BluetoothFindFirstRadio(&params, &hRadio);
  if (!hFind) return "00:00:00:00:00:00";
  BLUETOOTH_RADIO_INFO info = { sizeof(BLUETOOTH_RADIO_INFO) };
  if (BluetoothGetRadioInfo(hRadio, &info) == ERROR_SUCCESS) {
    char buf[18];
    snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
      info.address.rgBytes[5], info.address.rgBytes[4], info.address.rgBytes[3],
      info.address.rgBytes[2], info.address.rgBytes[1], info.address.rgBytes[0]);
    BluetoothFindRadioClose(hFind);
    CloseHandle(hRadio);
    return std::string(buf);
  }
  BluetoothFindRadioClose(hFind);
  CloseHandle(hRadio);
  return "00:00:00:00:00:00";
}

std::string Fl_Bluetooth::local_name() {
  HANDLE hRadio;
  BLUETOOTH_FIND_RADIO_PARAMS params = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };
  HBLUETOOTH_RADIO_FIND hFind = BluetoothFindFirstRadio(&params, &hRadio);
  if (!hFind) return "Windows-Bluetooth";
  BLUETOOTH_RADIO_INFO info = { sizeof(BLUETOOTH_RADIO_INFO) };
  if (BluetoothGetRadioInfo(hRadio, &info) == ERROR_SUCCESS) {
    char name[256];
    WideCharToMultiByte(CP_UTF8, 0, info.szName, -1, name, sizeof(name), NULL, NULL);
    BluetoothFindRadioClose(hFind);
    CloseHandle(hRadio);
    return std::string(name);
  }
  BluetoothFindRadioClose(hFind);
  CloseHandle(hRadio);
  return "Windows-Bluetooth";
}

void Fl_Bluetooth::power_on() {
  // Not supported via standard user-mode APIs on Windows without elevate
}

void Fl_Bluetooth::set_host_mode(HostMode mode) {
  (void)mode;
}

Fl_Bluetooth::HostMode Fl_Bluetooth::host_mode() {
  return HostConnectable;
}

std::vector<Fl_Bluetooth_Device> Fl_Bluetooth::scan_devices(int timeout_ms) {
  std::vector<Fl_Bluetooth_Device> devices;
  BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams = {
    sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS),
    1, 1, 1, 1, 1,
    (UCHAR)(timeout_ms / 1280),
    NULL
  };
  if (searchParams.cTimeoutMultiplier == 0) searchParams.cTimeoutMultiplier = 1;
  if (searchParams.cTimeoutMultiplier > 48) searchParams.cTimeoutMultiplier = 48;
  
  BLUETOOTH_DEVICE_INFO deviceInfo = { sizeof(BLUETOOTH_DEVICE_INFO) };
  HBLUETOOTH_DEVICE_FIND hFind = BluetoothFindFirstDevice(&searchParams, &deviceInfo);
  if (hFind) {
    do {
      Fl_Bluetooth_Device dev;
      char buf[18];
      snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
        deviceInfo.Address.rgBytes[5], deviceInfo.Address.rgBytes[4], deviceInfo.Address.rgBytes[3],
        deviceInfo.Address.rgBytes[2], deviceInfo.Address.rgBytes[1], deviceInfo.Address.rgBytes[0]);
      dev.address = buf;
      char name[256];
      WideCharToMultiByte(CP_UTF8, 0, deviceInfo.szName, -1, name, sizeof(name), NULL, NULL);
      dev.name = name;
      devices.push_back(dev);
    } while (BluetoothFindNextDevice(hFind, &deviceInfo));
    BluetoothFindDeviceClose(hFind);
  }
  return devices;
}

int Fl_Bluetooth::connect(const char* address, int channel) {
  if (is_open()) return -1;
  
  SOCKET s = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
  if (s == INVALID_SOCKET) return -1;

  SOCKADDR_BTH addr = { 0 };
  addr.addressFamily = AF_BTH;
  addr.port = channel;
  
  int str_len = (int)strlen(address);
  // Simple MAC parsing could go here, or we use WSAStringToAddress
  // Using a stub format for now if address parsing is complex
  // In a real implementation, parse "00:11:22:33:44:55" into BTH_ADDR
  
  BTH_ADDR bthAddr = 0;
  int parsed[6];
  if (sscanf(address, "%x:%x:%x:%x:%x:%x", 
             &parsed[0], &parsed[1], &parsed[2], &parsed[3], &parsed[4], &parsed[5]) == 6) {
    for (int i = 0; i < 6; i++) {
      bthAddr = (bthAddr << 8) | (parsed[i] & 0xFF);
    }
  }
  addr.btAddr = bthAddr;

  if (::connect(s, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
    ::closesocket(s);
    return -1;
  }

  socket_ = (uintptr_t)s;
  return 0;
}

int Fl_Bluetooth::close() {
  if (!is_open()) return -1;
  ::closesocket((SOCKET)socket_);
  socket_ = (uintptr_t)INVALID_SOCKET;
  return 0;
}

int Fl_Bluetooth::is_open() const {
  return socket_ != (uintptr_t)INVALID_SOCKET;
}

int Fl_Bluetooth::write_data(const void* data, int len) {
  if (!is_open()) return -1;
  return ::send((SOCKET)socket_, (const char*)data, len, 0);
}

int Fl_Bluetooth::read_data(void* buffer, int len) {
  if (!is_open()) return -1;
  return ::recv((SOCKET)socket_, (char*)buffer, len, 0);
}

#elif defined(__linux__)

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Avoid direct dependency on libbluetooth headers if they are missing
// We manually define necessary constants for AF_BLUETOOTH
#ifndef AF_BLUETOOTH
#define AF_BLUETOOTH 31
#endif
#ifndef BTPROTO_RFCOMM
#define BTPROTO_RFCOMM 3
#endif

// RFCOMM sockaddr structure
struct sockaddr_rc {
  sa_family_t rc_family;
  uint8_t rc_bdaddr[6];
  uint8_t rc_channel;
};

#define HCIGETDEVLIST _IOR('H', 210, int)
#define HCIGETDEVINFO _IOR('H', 211, int)

struct fl_hci_dev_req {
  uint16_t dev_id;
  uint32_t dev_opt;
};
struct fl_hci_dev_list_req {
  uint16_t dev_num;
  struct fl_hci_dev_req dev_req[16];
};
struct fl_hci_dev_info {
  uint16_t dev_id;
  char     name[8];
  uint8_t  bdaddr[6];
  uint8_t  padding[256];
};

Fl_Bluetooth::Fl_Bluetooth() : socket_(-1) {}

Fl_Bluetooth::~Fl_Bluetooth() {
  close();
}

std::string Fl_Bluetooth::local_address() {
  int sock = ::socket(AF_BLUETOOTH, SOCK_RAW, 1 /* BTPROTO_HCI */);
  if (sock < 0) return "00:00:00:00:00:00";
  struct fl_hci_dev_list_req dl;
  dl.dev_num = 16;
  if (ioctl(sock, HCIGETDEVLIST, (void *)&dl) < 0 || dl.dev_num == 0) {
    ::close(sock);
    return "00:00:00:00:00:00";
  }
  struct fl_hci_dev_info di;
  di.dev_id = dl.dev_req[0].dev_id;
  if (ioctl(sock, HCIGETDEVINFO, (void *)&di) < 0) {
    ::close(sock);
    return "00:00:00:00:00:00";
  }
  ::close(sock);
  char buf[18];
  snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
           di.bdaddr[5], di.bdaddr[4], di.bdaddr[3],
           di.bdaddr[2], di.bdaddr[1], di.bdaddr[0]);
  return std::string(buf);
}

std::string Fl_Bluetooth::local_name() {
  int sock = ::socket(AF_BLUETOOTH, SOCK_RAW, 1 /* BTPROTO_HCI */);
  if (sock < 0) return "Linux-Bluetooth";
  struct fl_hci_dev_list_req dl;
  dl.dev_num = 16;
  if (ioctl(sock, HCIGETDEVLIST, (void *)&dl) < 0 || dl.dev_num == 0) {
    ::close(sock);
    return "Linux-Bluetooth";
  }
  struct fl_hci_dev_info di;
  di.dev_id = dl.dev_req[0].dev_id;
  if (ioctl(sock, HCIGETDEVINFO, (void *)&di) < 0) {
    ::close(sock);
    return "Linux-Bluetooth";
  }
  ::close(sock);
  char name[256];
  snprintf(name, sizeof(name), "%s", di.name);
  return std::string(name);
}

void Fl_Bluetooth::power_on() {
  // Can be implemented using HCI ioctls
}

void Fl_Bluetooth::set_host_mode(HostMode mode) {
  (void)mode;
}

Fl_Bluetooth::HostMode Fl_Bluetooth::host_mode() {
  return HostConnectable;
}

std::vector<Fl_Bluetooth_Device> Fl_Bluetooth::scan_devices(int timeout_ms) {
  (void)timeout_ms;
  std::vector<Fl_Bluetooth_Device> devices;
  // Use bluetoothctl if available
  FILE* fp = popen("bluetoothctl devices 2>/dev/null", "r");
  if (!fp) return devices;
  char line[256];
  while (fgets(line, sizeof(line), fp)) {
    // Expected format: Device 00:11:22:33:44:55 Name...
    if (strncmp(line, "Device ", 7) == 0) {
      char addr[18] = {0};
      char name[200] = {0};
      if (sscanf(line + 7, "%17s %199[^\n]", addr, name) >= 1) {
        Fl_Bluetooth_Device dev;
        dev.address = addr;
        dev.name = name;
        devices.push_back(dev);
      }
    }
  }
  pclose(fp);
  return devices;
}

int Fl_Bluetooth::connect(const char* address, int channel) {
  if (is_open()) return -1;
  
  socket_ = ::socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
  if (socket_ < 0) return -1;

  struct sockaddr_rc addr = { 0 };
  addr.rc_family = AF_BLUETOOTH;
  addr.rc_channel = (uint8_t) channel;

  int parsed[6];
  if (sscanf(address, "%x:%x:%x:%x:%x:%x", 
             &parsed[0], &parsed[1], &parsed[2], &parsed[3], &parsed[4], &parsed[5]) == 6) {
    for (int i = 0; i < 6; i++) {
      addr.rc_bdaddr[5-i] = (uint8_t)(parsed[i] & 0xFF); // BlueZ expects reverse byte order
    }
  }

  if (::connect(socket_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    ::close(socket_);
    socket_ = -1;
    return -1;
  }

  return 0;
}

int Fl_Bluetooth::close() {
  if (!is_open()) return -1;
  ::close(socket_);
  socket_ = -1;
  return 0;
}

int Fl_Bluetooth::is_open() const {
  return socket_ >= 0;
}

int Fl_Bluetooth::write_data(const void* data, int len) {
  if (!is_open()) return -1;
  return ::write(socket_, data, len);
}

int Fl_Bluetooth::read_data(void* buffer, int len) {
  if (!is_open()) return -1;
  return ::read(socket_, buffer, len);
}

#else // macOS / other

#include <stdio.h>
#include <string.h>

Fl_Bluetooth::Fl_Bluetooth() : socket_(-1) {}
Fl_Bluetooth::~Fl_Bluetooth() {}

std::string Fl_Bluetooth::local_address() {
#ifdef __APPLE__
  FILE* fp = popen("system_profiler SPBluetoothDataType 2>/dev/null", "r");
  if (!fp) return "00:00:00:00:00:00";
  char line[256];
  std::string addr = "00:00:00:00:00:00";
  while (fgets(line, sizeof(line), fp)) {
    if (strstr(line, "Address: ")) {
      char* p = strstr(line, "Address: ") + 9;
      char* end = strchr(p, '\n');
      if (end) *end = 0;
      addr = p;
      // Convert dashes to colons if needed (Apple uses 00-11-22-33-44-55)
      for (size_t i = 0; i < addr.length(); ++i) {
        if (addr[i] == '-') addr[i] = ':';
      }
      break;
    }
  }
  pclose(fp);
  return addr;
#else
  return "";
#endif
}

std::string Fl_Bluetooth::local_name() {
#ifdef __APPLE__
  FILE* fp = popen("system_profiler SPBluetoothDataType 2>/dev/null", "r");
  if (!fp) return "Mac-Bluetooth";
  char line[256];
  std::string name = "Mac-Bluetooth";
  while (fgets(line, sizeof(line), fp)) {
    // Usually Name is just before Address, but simpler to just grep for it or use scutil
    // Better to use scutil --get ComputerName
    pclose(fp);
    fp = popen("scutil --get ComputerName 2>/dev/null", "r");
    if (fp && fgets(line, sizeof(line), fp)) {
      char* end = strchr(line, '\n');
      if (end) *end = 0;
      name = line;
    }
    if (fp) pclose(fp);
    return name;
  }
  if (fp) pclose(fp);
  return name;
#else
  return "";
#endif
}
void Fl_Bluetooth::power_on() {}
void Fl_Bluetooth::set_host_mode(HostMode mode) { (void)mode; }
Fl_Bluetooth::HostMode Fl_Bluetooth::host_mode() { return HostPoweredOff; }
std::vector<Fl_Bluetooth_Device> Fl_Bluetooth::scan_devices(int timeout_ms) { (void)timeout_ms; return std::vector<Fl_Bluetooth_Device>(); }
int Fl_Bluetooth::connect(const char* address, int channel) { (void)address; (void)channel; return -1; }
int Fl_Bluetooth::close() { return -1; }
int Fl_Bluetooth::is_open() const { return 0; }
int Fl_Bluetooth::write_data(const void* data, int len) { (void)data; (void)len; return -1; }
int Fl_Bluetooth::read_data(void* buffer, int len) { (void)buffer; (void)len; return -1; }

#endif
