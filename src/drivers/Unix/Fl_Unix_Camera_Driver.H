//
// Fl_Unix_Camera_Driver class for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#ifndef Fl_Unix_Camera_Driver_H
#define Fl_Unix_Camera_Driver_H

#include "../../Fl_Camera_Driver.H"
#include <FL/fl_attr.h>
#include <pthread.h>

class Fl_Unix_Camera_Driver : public Fl_Camera_Driver {
  int fd_;
  pthread_t thread_;
  int running_;
  
  // V4L2 buffer management
  struct buffer {
    void   *start;
    size_t  length;
  };
  struct buffer *buffers_;
  unsigned int n_buffers_;

  static void* capture_thread(void* arg);
  void process_frame();
  int init_device();
  void uninit_device();
  int start_capturing();
  void stop_capturing();

public:
  Fl_Unix_Camera_Driver(Fl_Camera *widget);
  virtual ~Fl_Unix_Camera_Driver();

  int start() FL_OVERRIDE;
  void stop() FL_OVERRIDE;
  Fl_RGB_Image* get_frame() FL_OVERRIDE;
};

#endif
