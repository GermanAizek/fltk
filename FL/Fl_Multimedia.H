//
// Fl_Multimedia widget and media player for the Fast Light Tool Kit (FLTK).
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

/** \file
   Fl_Multimedia class.
*/

#ifndef Fl_Multimedia_H
#define Fl_Multimedia_H

#include "Fl_Export.H"
#include "Fl_Widget.H"
#include <stdint.h>

class Fl_Multimedia_Driver;

/**
  \class Fl_Multimedia
  \brief Cross-platform multimedia player and widget providing Qt-like multimedia capabilities.

  This class offers high-level multimedia playback functionality (audio and video),
  analogous to `QMediaPlayer` in Qt Multimedia. It supports media source loading,
  playback states (Playing, Paused, Stopped), media status reporting, seeking, volume
  and mute controls, playback rate adjustments, looping, error handling, and rich
  callback hooks for state/position/duration/status/error changes.

  It can be used headlessly or embedded as a widget in a window to display media
  progress, title, playback controls, and status.
*/
class FL_EXPORT Fl_Multimedia : public Fl_Widget {
public:
  /** Playback states, matching Qt's QMediaPlayer::PlaybackState */
  enum PlaybackState {
    StoppedState = 0, ///< The media player is not playing media
    PlayingState = 1, ///< The media player is currently playing media
    PausedState  = 2  ///< The media player has paused playback
  };

  /** Media status states, matching Qt's QMediaPlayer::MediaStatus */
  enum MediaStatus {
    NoMedia        = 0, ///< No media source is currently loaded
    LoadingMedia   = 1, ///< The media is currently loading
    LoadedMedia    = 2, ///< The media is loaded and ready to play
    StalledMedia   = 3, ///< Media playback has stalled
    BufferingMedia = 4, ///< The player is currently buffering data
    BufferedMedia  = 5, ///< The player has buffered enough data
    EndOfMedia     = 6, ///< Playback has reached the end of the media
    InvalidMedia   = 7  ///< The media source is invalid or unsupported
  };

  /** Error codes, matching Qt's QMediaPlayer::Error */
  enum Error {
    NoError              = 0, ///< No error has occurred
    ResourceError        = 1, ///< The media resource could not be resolved or opened
    FormatError          = 2, ///< The media format is unsupported or corrupted
    NetworkError         = 3, ///< A network error occurred
    AccessDeniedError    = 4, ///< Access to the media was denied
    ServiceMissingError  = 5  ///< A required platform multimedia backend/service is missing
  };

  /** Playback loop count constants */
  enum LoopCount {
    Once     = 1,  ///< Play media once
    Infinite = -1  ///< Play media continuously in a loop
  };

  /** Callback type definitions */
  typedef void (*Fl_Multimedia_State_Cb)(Fl_Multimedia *player, PlaybackState state, void *data);
  typedef void (*Fl_Multimedia_Status_Cb)(Fl_Multimedia *player, MediaStatus status, void *data);
  typedef void (*Fl_Multimedia_Position_Cb)(Fl_Multimedia *player, int64_t position_ms, void *data);
  typedef void (*Fl_Multimedia_Duration_Cb)(Fl_Multimedia *player, int64_t duration_ms, void *data);
  typedef void (*Fl_Multimedia_Error_Cb)(Fl_Multimedia *player, Error err, const char *err_msg, void *data);

private:
  Fl_Multimedia_Driver *driver_;
  char *source_;
  PlaybackState playback_state_;
  MediaStatus media_status_;
  Error error_;
  char *error_string_;
  int64_t duration_ms_;
  int64_t position_ms_;
  double volume_;
  bool muted_;
  double playback_rate_;
  int loops_;
  int loops_remaining_;

  Fl_Multimedia_State_Cb state_cb_;
  void *state_data_;
  Fl_Multimedia_Status_Cb status_cb_;
  void *status_data_;
  Fl_Multimedia_Position_Cb position_cb_;
  void *position_data_;
  Fl_Multimedia_Duration_Cb duration_cb_;
  void *duration_data_;
  Fl_Multimedia_Error_Cb error_cb_;
  void *error_data_;

  void init_defaults();

protected:
  /** Draws the media player visualization and status */
  void draw() FL_OVERRIDE;

  /** Handles user interaction on the timeline and playback controls */
  int handle(int event) FL_OVERRIDE;

public:
  /**
    Creates a visual Fl_Multimedia widget.
    \param[in] X, Y, W, H the dimensions and position
    \param[in] L an optional label
  */
  Fl_Multimedia(int X, int Y, int W, int H, const char *L = 0);

  /**
    Creates a headless Fl_Multimedia player without visual dimensions.
  */
  Fl_Multimedia();

  /**
    Destroys the multimedia player and releases all allocated resources.
  */
  virtual ~Fl_Multimedia();

  // --- Source Management ---

  /**
    Sets the media source path or URL (e.g. "sample.wav", "/path/to/media.mp3", "http://...").
    \param[in] url_or_path The media source string.
  */
  void set_source(const char *url_or_path);

  /**
    Returns the current media source path or URL.
  */
  const char* source() const { return source_; }

  /**
    Alias for set_source() to match Qt QMediaPlayer::setMedia().
  */
  void set_media(const char *url_or_path) { set_source(url_or_path); }

  /**
    Alias for source() to match Qt QMediaPlayer::media().
  */
  const char* media() const { return source(); }

  // --- Playback Control ---

  /**
    Starts or resumes media playback.
    \return 1 on success, 0 on failure.
  */
  int play();

  /**
    Pauses media playback.
  */
  void pause();

  /**
    Stops media playback and resets playback position to 0.
  */
  void stop();

  /**
    Returns the current playback state (StoppedState, PlayingState, PausedState).
  */
  PlaybackState playback_state() const { return playback_state_; }

  /**
    Alias for playback_state() for Qt compatibility.
  */
  PlaybackState state() const { return playback_state_; }

  /**
    Returns the current media status.
  */
  MediaStatus media_status() const { return media_status_; }

  /**
    Alias for media_status() for Qt compatibility.
  */
  MediaStatus status() const { return media_status_; }

  // --- Position & Duration ---

  /**
    Returns the total duration of the media in milliseconds.
  */
  int64_t duration() const { return duration_ms_; }

  /**
    Returns the current playback position in milliseconds.
  */
  int64_t position() const { return position_ms_; }

  /**
    Seeks to a specific position in milliseconds.
    \param[in] position_ms The target position in milliseconds.
  */
  void set_position(int64_t position_ms);

  /**
    Alias for set_position().
  */
  void seek(int64_t position_ms) { set_position(position_ms); }

  /**
    Returns whether the current media supports seeking.
  */
  bool is_seekable() const;

  // --- Audio Properties ---

  /**
    Returns the current audio volume from 0.0 (silent) to 1.0 (full volume).
  */
  double volume() const { return volume_; }

  /**
    Sets the audio volume.
    \param[in] vol Volume level clamped between 0.0 and 1.0.
  */
  void set_volume(double vol);

  /**
    Returns true if audio playback is muted.
  */
  bool is_muted() const { return muted_; }

  /**
    Mutes or unmutes audio playback.
  */
  void set_muted(bool muted);

  /**
    Returns the current playback rate multiplier (1.0 = normal speed).
  */
  double playback_rate() const { return playback_rate_; }

  /**
    Sets the playback rate multiplier.
    \param[in] rate Speed multiplier (e.g. 0.5 for half speed, 1.0 normal, 2.0 double).
  */
  void set_playback_rate(double rate);

  /**
    Returns the configured loop count (1 for once, -1 for infinite).
  */
  int loops() const { return loops_; }

  /**
    Sets how many times media playback should repeat.
  */
  void set_loops(int count);

  // --- Media Information ---

  /**
    Returns true if the current media contains an audio stream.
  */
  bool has_audio() const;

  /**
    Returns true if the current media contains a video stream.
  */
  bool has_video() const;

  /**
    Returns true if the media player backend is available on this platform.
  */
  bool is_available() const;

  /**
    Returns the last error code.
  */
  Error error() const { return error_; }

  /**
    Returns a human-readable description of the last error.
  */
  const char* error_string() const { return error_string_ ? error_string_ : ""; }

  // --- Callbacks ---

  /** Sets callback for playback state changes. */
  void playback_state_changed_callback(Fl_Multimedia_State_Cb cb, void *data = 0);

  /** Sets callback for media status changes. */
  void media_status_changed_callback(Fl_Multimedia_Status_Cb cb, void *data = 0);

  /** Sets callback for position updates during playback. */
  void position_changed_callback(Fl_Multimedia_Position_Cb cb, void *data = 0);

  /** Sets callback for media duration changes. */
  void duration_changed_callback(Fl_Multimedia_Duration_Cb cb, void *data = 0);

  /** Sets callback for error notifications. */
  void error_callback(Fl_Multimedia_Error_Cb cb, void *data = 0);

  // --- Driver Notification Interface ---

  /** Internal method to update playback state and trigger callbacks. */
  void set_playback_state_internal(PlaybackState state);

  /** Internal method to update media status and trigger callbacks. */
  void set_media_status_internal(MediaStatus status);

  /** Internal method to update playback position and trigger callbacks. */
  void set_position_internal(int64_t position_ms);

  /** Internal method to update duration and trigger callbacks. */
  void set_duration_internal(int64_t duration_ms);

  /** Internal method to set error state and trigger error callback. */
  void set_error(Error err, const char *msg);

  /** Internal method invoked when end of media is reached to handle loops. */
  void on_end_of_media();
};

#include "Fl_Camera.H"
#include "Fl_Audio_Input.H"
#include "Fl_Audio_Output.H"
#include "Fl_Video_Sink.H"
#include "Fl_Audio_Sink.H"
#include "Fl_Video_Widget.H"
#include "Fl_Image_Capture.H"
#include "Fl_Media_Recorder.H"
#include "Fl_Screen_Capture.H"
#include "Fl_Window_Capture.H"
#include "Fl_Video_Frame_Input.H"
#include "Fl_Audio_Buffer_Input.H"
#include "Fl_Media_Capture_Session.H"
#include "Fl_Media_Player.H"

#endif
