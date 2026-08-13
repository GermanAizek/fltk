//
// PCM class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_PCM_H
#define Fl_PCM_H

#include "Fl_Export.H"
#include <stdint.h>

/**
  Cross-platform PCM (Pulse Code Modulation) audio wrapper.
  
  This class provides a uniform interface to read and write PCM audio data.
  On Linux, it utilizes the OSS `/dev/dsp` interface for zero-dependency compilation.
  
  On other platforms (Windows, macOS), the methods act as stubs and return
  error codes (-1) when operations fail, allowing cross-platform
  compilation without conditional logic in user code.
*/
class FL_EXPORT Fl_PCM {
  int fd_;
  int sample_rate_;
  int channels_;
  int bit_depth_;

public:
  /**
    Initializes a PCM instance but does not open an audio device.
  */
  Fl_PCM();
  
  /**
    Cleans up the PCM instance, closing the device if open.
  */
  ~Fl_PCM();

  /**
    Opens a PCM audio device.
    \param device The path to the device, e.g., "/dev/dsp" on Linux.
    \param sample_rate The sample rate in Hz, e.g. 44100.
    \param channels The number of channels, e.g. 2 for stereo.
    \param bit_depth The number of bits per sample, e.g. 16.
    \param mode 0 for read (record), 1 for write (playback), 2 for read/write.
    \return 0 on success, or -1 on error.
  */
  int open(const char* device, int sample_rate = 44100, int channels = 2, int bit_depth = 16, int mode = 1);
  
  /**
    Closes the currently open PCM device.
    \return 0 on success, or -1 if no device is open.
  */
  int close();

  /**
    Checks if a PCM device is currently open.
    \return 1 if open, 0 if not.
  */
  int is_open() const;

  /**
    Writes PCM frames to the audio device (playback).
    \param data Pointer to the buffer containing PCM data.
    \param num_bytes Number of bytes to write.
    \return The number of bytes written, or -1 on error.
  */
  int write_frames(const void* data, int num_bytes);
  
  /**
    Reads PCM frames from the audio device (recording).
    \param data Pointer to the buffer to store read data.
    \param num_bytes Number of bytes to read.
    \return The number of bytes read, or -1 on error.
  */
  int read_frames(void* data, int num_bytes);
};

#endif
