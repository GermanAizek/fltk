//
// PPM (Pulse Position Modulation) class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_PPM_H
#define Fl_PPM_H

#include "Fl_Export.H"
#include <stdint.h>

class Fl_PPM;
typedef void (*Fl_PPM_Callback)(Fl_PPM*, void*);

/**
  Cross-platform Pulse Position Modulation (PPM) class.
  
  This class provides a uniform interface to configure, read, and write 
  PPM channel values. Typical PPM ranges for RC are 1000us to 2000us.
*/
class FL_EXPORT Fl_PPM {
private:
  Fl_PPM_Callback callback_;
  void* user_data_;

  int num_channels_;
  int frame_length_us_;
  int pulse_width_us_;
  uint16_t channels_[16]; // Maximum 16 channels

#if defined(_WIN32)
  void* handle_;
#else
  int fd_;
#endif

public:
  /**
    Initializes a PPM instance with default parameters.
  */
  Fl_PPM();
  
  /**
    Cleans up the PPM instance, closing if open.
  */
  ~Fl_PPM();

  /**
    Opens the PPM interface.
    \param device The path or name of the device/port.
    \return 0 on success, or -1 on error.
  */
  int open(const char* device);
  
  /**
    Closes the currently open interface.
    \return 0 on success, or -1 if no port is open.
  */
  int close();

  /**
    Checks if the PPM interface is open.
    \return 1 if open, 0 if not.
  */
  int is_open() const;

  /**
    Sets the number of channels (typically 8).
    \param count Number of channels (1 to 16).
  */
  void set_num_channels(int count);
  
  /**
    Gets the current number of channels.
  */
  int num_channels() const { return num_channels_; }

  /**
    Sets the total frame length in microseconds (e.g. 20000us = 20ms).
  */
  void set_frame_length(int us);

  /**
    Sets the sync pulse width in microseconds (e.g. 300us).
  */
  void set_pulse_width(int us);

  /**
    Writes multiple channel values (in microseconds).
    \param values Array of uint16_t.
    \param count Number of channels to write.
    \return Number of channels successfully written, or -1 on error.
  */
  int write_channels(const uint16_t* values, int count);
  
  /**
    Reads the current channel values.
    \param buffer Pointer to the buffer to store read data.
    \param max_count Maximum number of channels to read.
    \return Number of channels successfully read, or -1 on error.
  */
  int read_channels(uint16_t* buffer, int max_count);

  /**
    Sets the callback function to be called when new data is available.
    \param cb The callback function.
    \param user_data Optional user data passed to the callback.
  */
  void callback(Fl_PPM_Callback cb, void* user_data = nullptr);
  
  /**
    Triggers the callback manually.
  */
  void do_callback();
  
  /**
    Gets the current callback.
  */
  Fl_PPM_Callback callback() const { return callback_; }
  
  /**
    Gets the current user data.
  */
  void* user_data() const { return user_data_; }
  
  /**
    Sets the user data.
  */
  void user_data(void* data) { user_data_ = data; }
};

#endif
