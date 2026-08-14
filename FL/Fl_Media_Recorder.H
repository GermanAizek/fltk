//
// Fl_Media_Recorder class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Media_Recorder_H
#define Fl_Media_Recorder_H

#include "Fl_Export.H"
#include <stdint.h>

class Fl_Media_Capture_Session;

/**
  \class Fl_Media_Recorder
  \brief Records media content from a capture session, matching Qt's QMediaRecorder.
*/
class FL_EXPORT Fl_Media_Recorder {
public:
  enum RecorderState {
    StoppedState   = 0,
    RecordingState = 1,
    PausedState    = 2
  };

  enum Error {
    NoError         = 0,
    ResourceError   = 1,
    FormatError     = 2,
    OutOfSpaceError = 3
  };

  typedef void (*Fl_Recorder_State_Cb)(Fl_Media_Recorder *rec, RecorderState state, void *data);
  typedef void (*Fl_Recorder_Duration_Cb)(Fl_Media_Recorder *rec, int64_t duration_ms, void *data);
  typedef void (*Fl_Recorder_Error_Cb)(Fl_Media_Recorder *rec, Error err, const char *err_msg, void *data);

private:
  Fl_Media_Capture_Session *session_;
  RecorderState state_;
  Error error_;
  char *error_string_;
  char *output_location_;
  char *actual_location_;
  int64_t duration_ms_;

  Fl_Recorder_State_Cb state_cb_;
  void *state_data_;
  Fl_Recorder_Duration_Cb duration_cb_;
  void *duration_data_;
  Fl_Recorder_Error_Cb error_cb_;
  void *error_data_;

public:
  Fl_Media_Recorder();
  virtual ~Fl_Media_Recorder();

  /** Sets the capture session */
  void set_capture_session(Fl_Media_Capture_Session *session);

  /** Returns the capture session */
  Fl_Media_Capture_Session* capture_session() const { return session_; }

  /** Starts recording */
  void record();

  /** Pauses recording */
  void pause();

  /** Stops recording */
  void stop();

  /** Returns current recorder state */
  RecorderState recorder_state() const { return state_; }

  /** Sets requested output file path */
  void set_output_location(const char *path);

  /** Returns requested output file path */
  const char* output_location() const { return output_location_ ? output_location_ : ""; }

  /** Returns actual output location */
  const char* actual_location() const { return actual_location_ ? actual_location_ : output_location(); }

  /** Returns recorded duration in milliseconds */
  int64_t duration() const { return duration_ms_; }

  /** Sets recorder state changed callback */
  void recorder_state_changed_callback(Fl_Recorder_State_Cb cb, void *data = 0);

  /** Sets recorded duration changed callback */
  void duration_changed_callback(Fl_Recorder_Duration_Cb cb, void *data = 0);

  /** Sets error callback */
  void error_callback(Fl_Recorder_Error_Cb cb, void *data = 0);

  /** Returns last error code */
  Error error() const { return error_; }

  /** Returns last error string */
  const char* error_string() const { return error_string_ ? error_string_ : ""; }
};

#endif
