//
// DBus class for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_DBus.H>

#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

// Define necessary DBus types locally to avoid requiring dbus-1-dev
typedef struct DBusError {
  const char *name;
  const char *message;
  unsigned int dummy1;
  unsigned int dummy2;
  unsigned int dummy3;
  unsigned int dummy4;
  unsigned int dummy5;
  void *padding1;
} DBusError;

typedef enum {
  DBUS_BUS_SESSION,
  DBUS_BUS_SYSTEM,
  DBUS_BUS_STARTER
} DBusBusType;

typedef struct DBusConnection DBusConnection;
typedef struct DBusMessage DBusMessage;
typedef struct DBusMessageIter DBusMessageIter;

#define DBUS_TYPE_STRING ((int)'s')
#define DBUS_TYPE_INVALID ((int)'\0')

// Function pointers
static DBusConnection* (*dbus_bus_get)(DBusBusType type, DBusError *error) = 0;
static void (*dbus_error_init)(DBusError *error) = 0;
static void (*dbus_error_free)(DBusError *error) = 0;
static DBusMessage* (*dbus_message_new_method_call)(const char *destination, const char *path, const char *iface, const char *method) = 0;
static void (*dbus_message_unref)(DBusMessage *message) = 0;
static int (*dbus_message_append_args)(DBusMessage *message, int first_arg_type, ...) = 0;
static DBusMessage* (*dbus_connection_send_with_reply_and_block)(DBusConnection *connection, DBusMessage *message, int timeout_milliseconds, DBusError *error) = 0;
static int (*dbus_message_get_args)(DBusMessage *message, DBusError *error, int first_arg_type, ...) = 0;

static bool dbus_loaded = false;
static bool dbus_tried_load = false;

static void load_dbus() {
  if (dbus_tried_load) return;
  dbus_tried_load = true;

  void* handle = dlopen("libdbus-1.so.3", RTLD_LAZY);
  if (!handle) handle = dlopen("libdbus-1.so", RTLD_LAZY);
#ifdef __APPLE__
  if (!handle) handle = dlopen("libdbus-1.dylib", RTLD_LAZY);
#endif

  if (handle) {
    dbus_bus_get = (DBusConnection* (*)(DBusBusType, DBusError*))dlsym(handle, "dbus_bus_get");
    dbus_error_init = (void (*)(DBusError*))dlsym(handle, "dbus_error_init");
    dbus_error_free = (void (*)(DBusError*))dlsym(handle, "dbus_error_free");
    dbus_message_new_method_call = (DBusMessage* (*)(const char*, const char*, const char*, const char*))dlsym(handle, "dbus_message_new_method_call");
    dbus_message_unref = (void (*)(DBusMessage*))dlsym(handle, "dbus_message_unref");
    dbus_message_append_args = (int (*)(DBusMessage*, int, ...))dlsym(handle, "dbus_message_append_args");
    dbus_connection_send_with_reply_and_block = (DBusMessage* (*)(DBusConnection*, DBusMessage*, int, DBusError*))dlsym(handle, "dbus_connection_send_with_reply_and_block");
    dbus_message_get_args = (int (*)(DBusMessage*, DBusError*, int, ...))dlsym(handle, "dbus_message_get_args");

    if (dbus_bus_get && dbus_error_init && dbus_error_free && dbus_message_new_method_call &&
        dbus_message_unref && dbus_message_append_args && dbus_connection_send_with_reply_and_block &&
        dbus_message_get_args) {
      dbus_loaded = true;
    }
  }
}

Fl_DBus::Fl_DBus(BusType type) : connection_(0), type_(type), is_connected_(false) {
  load_dbus();
  if (dbus_loaded) {
    DBusError err;
    dbus_error_init(&err);
    DBusBusType bt = (type == SessionBus) ? DBUS_BUS_SESSION : DBUS_BUS_SYSTEM;
    connection_ = dbus_bus_get(bt, &err);
    if (connection_ != 0) {
      is_connected_ = true;
    }
    dbus_error_free(&err);
  }
}

Fl_DBus::~Fl_DBus() {
  // DBus usually handles shared connection cleanup automatically.
}

bool Fl_DBus::is_connected() const {
  return is_connected_;
}

std::string Fl_DBus::call_method(const char* service, 
                                 const char* path, 
                                 const char* interface, 
                                 const char* method, 
                                 const char* arg) {
  if (!is_connected_ || !dbus_loaded) return "";

  DBusMessage* msg = dbus_message_new_method_call(service, path, interface, method);
  if (!msg) return "";

  if (arg) {
    dbus_message_append_args(msg, DBUS_TYPE_STRING, &arg, DBUS_TYPE_INVALID);
  }

  DBusError err;
  dbus_error_init(&err);

  DBusMessage* reply = dbus_connection_send_with_reply_and_block((DBusConnection*)connection_, msg, -1, &err);
  dbus_message_unref(msg);

  std::string result = "";
  if (reply != 0) {
    char* str_res = 0;
    // Try to get a single string result
    if (dbus_message_get_args(reply, &err, DBUS_TYPE_STRING, &str_res, DBUS_TYPE_INVALID)) {
      if (str_res) result = str_res;
    } else {
       dbus_error_free(&err);
    }
    dbus_message_unref(reply);
  } else {
    dbus_error_free(&err);
  }

  return result;
}

#elif defined(_WIN32)

// Windows stub (DBus is not typically present on Windows, 
// a full implementation would dynamically load dbus-1.dll).
#include <windows.h>

Fl_DBus::Fl_DBus(BusType type) : connection_(0), type_(type), is_connected_(false) {}
Fl_DBus::~Fl_DBus() {}
bool Fl_DBus::is_connected() const { return false; }
std::string Fl_DBus::call_method(const char*, const char*, const char*, const char*, const char*) { return ""; }

#else

// Fallback for unknown platforms
Fl_DBus::Fl_DBus(BusType type) : connection_(0), type_(type), is_connected_(false) {}
Fl_DBus::~Fl_DBus() {}
bool Fl_DBus::is_connected() const { return false; }
std::string Fl_DBus::call_method(const char*, const char*, const char*, const char*, const char*) { return ""; }

#endif
