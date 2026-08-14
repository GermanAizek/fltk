//
// Fl_Media_Player class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Media_Player_H
#define Fl_Media_Player_H

#include "Fl_Export.H"
#include "Fl_Multimedia.H"
#include "Fl_Audio_Output.H"
#include "Fl_Video_Sink.H"
#include "Fl_Video_Widget.H"

/**
  \class Fl_Media_Player
  \brief High-level media playback controller matching Qt's QMediaPlayer.
*/
class FL_EXPORT Fl_Media_Player {
public:
  enum PlaybackState {
    StoppedState = Fl_Multimedia::StoppedState,
    PlayingState = Fl_Multimedia::PlayingState,
    PausedState  = Fl_Multimedia::PausedState
  };

  enum MediaStatus {
    NoMedia        = Fl_Multimedia::NoMedia,
    LoadingMedia   = Fl_Multimedia::LoadingMedia,
    LoadedMedia    = Fl_Multimedia::LoadedMedia,
    StalledMedia   = Fl_Multimedia::StalledMedia,
    BufferingMedia = Fl_Multimedia::BufferingMedia,
    BufferedMedia  = Fl_Multimedia::BufferedMedia,
    EndOfMedia     = Fl_Multimedia::EndOfMedia,
    InvalidMedia   = Fl_Multimedia::InvalidMedia
  };

  enum Error {
    NoError              = Fl_Multimedia::NoError,
    ResourceError        = Fl_Multimedia::ResourceError,
    FormatError          = Fl_Multimedia::FormatError,
    NetworkError         = Fl_Multimedia::NetworkError,
    AccessDeniedError    = Fl_Multimedia::AccessDeniedError,
    ServiceMissingError  = Fl_Multimedia::ServiceMissingError
  };

  enum LoopCount {
    Once     = Fl_Multimedia::Once,
    Infinite = Fl_Multimedia::Infinite
  };

  typedef Fl_Multimedia::Fl_Multimedia_State_Cb Fl_Player_State_Cb;
  typedef Fl_Multimedia::Fl_Multimedia_Status_Cb Fl_Player_Status_Cb;
  typedef Fl_Multimedia::Fl_Multimedia_Position_Cb Fl_Player_Position_Cb;
  typedef Fl_Multimedia::Fl_Multimedia_Duration_Cb Fl_Player_Duration_Cb;
  typedef Fl_Multimedia::Fl_Multimedia_Error_Cb Fl_Player_Error_Cb;

private:
  Fl_Multimedia player_engine_;
  Fl_Audio_Output *audio_output_;
  Fl_Video_Sink *video_sink_;
  Fl_Video_Widget *video_output_;

public:
  Fl_Media_Player();
  virtual ~Fl_Media_Player();

  /** Sets media source path or URL */
  void set_source(const char *url_or_path);

  /** Returns media source */
  const char* source() const { return player_engine_.source(); }

  /** Alias for set_source() */
  void set_media(const char *url_or_path) { set_source(url_or_path); }

  /** Alias for source() */
  const char* media() const { return source(); }

  /** Starts or resumes playback */
  int play() { return player_engine_.play(); }

  /** Pauses playback */
  void pause() { player_engine_.pause(); }

  /** Stops playback */
  void stop() { player_engine_.stop(); }

  /** Returns current playback state */
  PlaybackState playback_state() const { return (PlaybackState)player_engine_.playback_state(); }

  /** Returns current media status */
  MediaStatus media_status() const { return (MediaStatus)player_engine_.media_status(); }

  /** Returns media duration in milliseconds */
  int64_t duration() const { return player_engine_.duration(); }

  /** Returns playback position in milliseconds */
  int64_t position() const { return player_engine_.position(); }

  /** Seeks to a position in milliseconds */
  void set_position(int64_t pos_ms) { player_engine_.set_position(pos_ms); }

  /** Alias for set_position() */
  void seek(int64_t pos_ms) { set_position(pos_ms); }

  /** Returns audio volume (0.0 to 1.0) */
  double volume() const { return player_engine_.volume(); }

  /** Sets audio volume */
  void set_volume(double vol) { player_engine_.set_volume(vol); }

  /** Returns whether playback is muted */
  bool is_muted() const { return player_engine_.is_muted(); }

  /** Mutes or unmutes playback */
  void set_muted(bool muted) { player_engine_.set_muted(muted); }

  /** Returns playback rate */
  double playback_rate() const { return player_engine_.playback_rate(); }

  /** Sets playback rate */
  void set_playback_rate(double rate) { player_engine_.set_playback_rate(rate); }

  /** Returns loop count */
  int loops() const { return player_engine_.loops(); }

  /** Sets loop count */
  void set_loops(int count) { player_engine_.set_loops(count); }

  /** Returns true if media has audio stream */
  bool has_audio() const { return player_engine_.has_audio(); }

  /** Returns true if media has video stream */
  bool has_video() const { return player_engine_.has_video(); }

  /** Returns true if media is seekable */
  bool is_seekable() const { return player_engine_.is_seekable(); }

  /** Returns true if backend is available */
  bool is_available() const { return player_engine_.is_available(); }

  /** Returns last error code */
  Error error() const { return (Error)player_engine_.error(); }

  /** Returns last error string */
  const char* error_string() const { return player_engine_.error_string(); }

  /** Sets audio output device */
  void set_audio_output(Fl_Audio_Output *output);

  /** Returns current audio output device */
  Fl_Audio_Output* audio_output() const { return audio_output_; }

  /** Sets video sink */
  void set_video_sink(Fl_Video_Sink *sink);

  /** Returns current video sink */
  Fl_Video_Sink* video_sink() const { return video_sink_; }

  /** Sets video output widget */
  void set_video_output(Fl_Video_Widget *widget);

  /** Returns current video output widget */
  Fl_Video_Widget* video_output() const { return video_output_; }

  /** Sets playback state changed callback */
  void playback_state_changed_callback(Fl_Player_State_Cb cb, void *data = 0) {
    player_engine_.playback_state_changed_callback(cb, data);
  }

  /** Sets media status changed callback */
  void media_status_changed_callback(Fl_Player_Status_Cb cb, void *data = 0) {
    player_engine_.media_status_changed_callback(cb, data);
  }

  /** Sets position changed callback */
  void position_changed_callback(Fl_Player_Position_Cb cb, void *data = 0) {
    player_engine_.position_changed_callback(cb, data);
  }

  /** Sets duration changed callback */
  void duration_changed_callback(Fl_Player_Duration_Cb cb, void *data = 0) {
    player_engine_.duration_changed_callback(cb, data);
  }

  /** Sets error callback */
  void error_callback(Fl_Player_Error_Cb cb, void *data = 0) {
    player_engine_.error_callback(cb, data);
  }
};

typedef Fl_Media_Player Fl_MediaPlayer;

#endif
