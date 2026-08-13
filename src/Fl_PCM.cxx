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

#include <FL/Fl_PCM.H>

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#endif

Fl_PCM::Fl_PCM() : fd_(-1), sample_rate_(44100), channels_(2), bit_depth_(16) {}

Fl_PCM::~Fl_PCM() {
  close();
}

int Fl_PCM::open(const char* device, int sample_rate, int channels, int bit_depth, int mode) {
  close();
#ifdef __linux__
  int flags = O_WRONLY;
  if (mode == 0) flags = O_RDONLY;
  else if (mode == 2) flags = O_RDWR;

  fd_ = ::open(device, flags);
  if (fd_ < 0) return -1;

  int format = AFMT_S16_LE;
  if (bit_depth == 8) format = AFMT_U8;
  
  if (ioctl(fd_, SNDCTL_DSP_SETFMT, &format) == -1) {
    close();
    return -1;
  }
  
  int ch = channels;
  if (ioctl(fd_, SNDCTL_DSP_CHANNELS, &ch) == -1) {
    close();
    return -1;
  }
  
  int rate = sample_rate;
  if (ioctl(fd_, SNDCTL_DSP_SPEED, &rate) == -1) {
    close();
    return -1;
  }
  
  sample_rate_ = rate;
  channels_ = ch;
  bit_depth_ = (format == AFMT_U8) ? 8 : 16;
  
  return 0;
#else
  (void)device;
  (void)sample_rate;
  (void)channels;
  (void)bit_depth;
  (void)mode;
  return -1;
#endif
}

int Fl_PCM::close() {
  if (fd_ >= 0) {
#ifdef __linux__
    ::close(fd_);
#endif
    fd_ = -1;
    return 0;
  }
  return -1;
}

int Fl_PCM::is_open() const {
  return fd_ >= 0;
}

int Fl_PCM::write_frames(const void* data, int num_bytes) {
  if (fd_ < 0 || num_bytes < 0) return -1;
#ifdef __linux__
  return ::write(fd_, data, num_bytes);
#else
  (void)data;
  return -1;
#endif
}

int Fl_PCM::read_frames(void* data, int num_bytes) {
  if (fd_ < 0 || num_bytes < 0) return -1;
#ifdef __linux__
  return ::read(fd_, data, num_bytes);
#else
  (void)data;
  return -1;
#endif
}
