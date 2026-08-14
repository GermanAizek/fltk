//
// Fl_Image_Capture class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Image_Capture_H
#define Fl_Image_Capture_H

#include "Fl_Export.H"
#include <FL/Fl_Image.H>

class Fl_Media_Capture_Session;

/**
  \class Fl_Image_Capture
  \brief Captures still images from a camera or capture session, matching Qt's QImageCapture.
*/
class FL_EXPORT Fl_Image_Capture {
public:
  enum Error {
    NoError        = 0,
    NotReadyError  = 1,
    ResourceError  = 2,
    OutOfMemoryError = 3
  };

  typedef void (*Fl_Image_Captured_Cb)(Fl_Image_Capture *cap, int id, Fl_RGB_Image *image, void *data);
  typedef void (*Fl_Image_Saved_Cb)(Fl_Image_Capture *cap, int id, const char *filename, void *data);
  typedef void (*Fl_Image_Capture_Error_Cb)(Fl_Image_Capture *cap, int id, Error error, const char *error_str, void *data);

private:
  Fl_Media_Capture_Session *session_;
  Fl_RGB_Image *last_image_;
  int next_id_;
  bool ready_;
  Error error_;
  char *error_string_;

  Fl_Image_Captured_Cb captured_cb_;
  void *captured_data_;
  Fl_Image_Saved_Cb saved_cb_;
  void *saved_data_;
  Fl_Image_Capture_Error_Cb error_cb_;
  void *error_data_;

public:
  Fl_Image_Capture();
  virtual ~Fl_Image_Capture();

  /** Sets associated capture session */
  void set_capture_session(Fl_Media_Capture_Session *session);

  /** Returns associated capture session */
  Fl_Media_Capture_Session* capture_session() const { return session_; }

  /** Returns true if image capture is ready */
  bool is_ready_for_capture() const { return ready_; }

  /** Captures a still frame to memory, returns unique ID */
  int capture();

  /** Captures a still frame and saves it to a file, returns unique ID */
  int capture_to_file(const char *file_path = 0);

  /** Returns the last captured image */
  Fl_RGB_Image* last_image() const { return last_image_; }

  /** Returns the last error */
  Error error() const { return error_; }

  /** Returns the last error string */
  const char* error_string() const { return error_string_ ? error_string_ : ""; }

  /** Sets image captured callback */
  void image_captured_callback(Fl_Image_Captured_Cb cb, void *data = 0);

  /** Sets image saved to file callback */
  void image_saved_callback(Fl_Image_Saved_Cb cb, void *data = 0);

  /** Sets error callback */
  void error_callback(Fl_Image_Capture_Error_Cb cb, void *data = 0);

  /** Internal method to set the latest captured frame */
  void set_captured_frame(Fl_RGB_Image *frame, int id = 0, const char *saved_file = 0);
};

#endif
