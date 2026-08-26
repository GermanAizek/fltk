//
// ARINC 818 (ADVB - Avionics Digital Video Bus) Protocol class for FLTK.
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.
//

#ifndef Fl_ARINC818_H
#define Fl_ARINC818_H

#include "Fl_Export.H"
#include <FL/Fl_Serial_Port.H>
#include <stdint.h>
#include <stddef.h>
#include <vector>

class Fl_ARINC818;
typedef void (*Fl_ARINC818_Callback)(Fl_ARINC818*, void*);

/**
  Cross-platform ARINC 818 (ADVB) Video & Ancillary Telemetry Container Parser.
  
  Decodes ADVB container metadata:
  - Container Header & Object ID (Object 0: Container Header, Object 1: Video, Object 2: Ancillary Data, Object 3: Audio)
  - Video parameters: resolution width/height, frame rate, color format (RGB888, YCbCr422, Monochrome)
  - Ancillary telemetry data payload (HUD overlay symbology, FLIR target tracking telemetry)
*/
class FL_EXPORT Fl_ARINC818 : public Fl_Serial_Port {
public:
  struct ContainerInfo {
    uint32_t frame_count;          // Monotonic frame counter
    uint16_t video_width;          // Horizontal pixels (e.g. 1920, 1024)
    uint16_t video_height;         // Vertical pixels (e.g. 1080, 768)
    uint16_t frame_rate_fps;       // 30, 60, 120 fps
    uint16_t ancillary_len;        // Ancillary telemetry bytes
    uint8_t  container_ver;        // ARINC 818 revision (e.g. 1 or 2)
    uint8_t  color_format;         // 0: RGB, 1: YCbCr, 2: Monochrome
    bool     is_valid;
    std::vector<uint8_t> ancillary_data;
  };

private:
  ContainerInfo last_container_;
  Fl_ARINC818_Callback arinc818_cb_;
  void* user_data_;
  std::vector<uint8_t> buffer_;

  static void serial_cb(Fl_Serial_Port* p, void* data);
  void process_byte(uint8_t b);

public:
  Fl_ARINC818();
  virtual ~Fl_ARINC818();

  int open(const char* port_name);
  void arinc818_callback(Fl_ARINC818_Callback cb, void* user_data = nullptr);

  const ContainerInfo& last_container() const { return last_container_; }

  void feed_container(uint16_t width, uint16_t height, uint16_t fps, uint32_t frame_num, const uint8_t* ancillary, uint16_t anc_len);
};

#endif
