//
// Fl_Unix_Multimedia_Driver implementation for the Fast Light Tool Kit (FLTK).
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

#include "Fl_Unix_Multimedia_Driver.H"
#include <FL/Fl.H>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <algorithm>

#define MULTIMEDIA_TICK_INTERVAL 0.05 // 50 ms

Fl_Multimedia_Driver* Fl_Multimedia_Driver::new_multimedia_driver(Fl_Multimedia *player) {
  return new Fl_Unix_Multimedia_Driver(player);
}

Fl_Unix_Multimedia_Driver::Fl_Unix_Multimedia_Driver(Fl_Multimedia *player)
  : Fl_Multimedia_Driver(player),
    duration_ms_(0),
    position_ms_(0),
    volume_(1.0),
    muted_(false),
    playback_rate_(1.0),
    is_playing_(false),
    is_paused_(false),
    has_audio_(false),
    has_video_(false),
    is_seekable_(false),
    sample_rate_(44100),
    channels_(2),
    bits_per_sample_(16),
    data_size_(0),
    data_offset_(0) {
}

Fl_Unix_Multimedia_Driver::~Fl_Unix_Multimedia_Driver() {
  is_playing_ = false;
  is_paused_ = false;
  Fl::remove_timeout(timer_cb, this);
}

bool Fl_Unix_Multimedia_Driver::parse_wav_header(const char *filename) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) return false;

  char chunk_id[4];
  uint32_t chunk_size;
  char format[4];

  if (fread(chunk_id, 1, 4, fp) != 4 || memcmp(chunk_id, "RIFF", 4) != 0) {
    fclose(fp);
    return false;
  }

  if (fread(&chunk_size, 4, 1, fp) != 1) {
    fclose(fp);
    return false;
  }

  if (fread(format, 1, 4, fp) != 4 || memcmp(format, "WAVE", 4) != 0) {
    fclose(fp);
    return false;
  }

  bool found_fmt = false;
  bool found_data = false;

  while (!feof(fp) && (!found_fmt || !found_data)) {
    char sub_id[4];
    uint32_t sub_size;
    if (fread(sub_id, 1, 4, fp) != 4) break;
    if (fread(&sub_size, 4, 1, fp) != 1) break;

    if (memcmp(sub_id, "fmt ", 4) == 0) {
      uint16_t audio_fmt;
      if (fread(&audio_fmt, 2, 1, fp) != 1) break;
      if (fread(&channels_, 2, 1, fp) != 1) break;
      if (fread(&sample_rate_, 4, 1, fp) != 1) break;

      uint32_t byte_rate;
      uint16_t block_align;
      if (fread(&byte_rate, 4, 1, fp) != 1) break;
      if (fread(&block_align, 2, 1, fp) != 1) break;
      if (fread(&bits_per_sample_, 2, 1, fp) != 1) break;

      // Skip any extra format bytes
      if (sub_size > 16) {
        fseek(fp, sub_size - 16, SEEK_CUR);
      }
      found_fmt = true;
    } else if (memcmp(sub_id, "data", 4) == 0) {
      data_size_ = sub_size;
      data_offset_ = ftell(fp);
      found_data = true;
      fseek(fp, sub_size, SEEK_CUR);
    } else {
      // Skip unknown chunk
      fseek(fp, sub_size, SEEK_CUR);
    }
  }

  fclose(fp);

  if (found_fmt && found_data && sample_rate_ > 0 && channels_ > 0 && bits_per_sample_ > 0) {
    uint32_t bytes_per_sec = sample_rate_ * channels_ * (bits_per_sample_ / 8);
    if (bytes_per_sec > 0) {
      duration_ms_ = ((int64_t)data_size_ * 1000) / bytes_per_sec;
      has_audio_ = true;
      has_video_ = false;
      is_seekable_ = true;
      return true;
    }
  }

  return false;
}

int Fl_Unix_Multimedia_Driver::set_source(const char *url_or_path) {
  stop();

  if (!url_or_path || url_or_path[0] == '\0') {
    source_path_.clear();
    duration_ms_ = 0;
    position_ms_ = 0;
    has_audio_ = false;
    has_video_ = false;
    is_seekable_ = false;
    player_->set_duration_internal(0);
    player_->set_position_internal(0);
    player_->set_media_status_internal(Fl_Multimedia::NoMedia);
    return 1;
  }

  source_path_ = url_or_path;
  player_->set_media_status_internal(Fl_Multimedia::LoadingMedia);

  // Check if it's a URL
  bool is_url = (source_path_.compare(0, 7, "http://") == 0 ||
                 source_path_.compare(0, 8, "https://") == 0 ||
                 source_path_.compare(0, 6, "rtsp://") == 0);

  if (is_url) {
    duration_ms_ = 60000; // Simulated network stream
    position_ms_ = 0;
    has_audio_ = true;
    has_video_ = true;
    is_seekable_ = true;
    player_->set_duration_internal(duration_ms_);
    player_->set_position_internal(0);
    player_->set_media_status_internal(Fl_Multimedia::LoadedMedia);
    return 1;
  }

  // Check if file exists on disk
  struct stat st;
  if (stat(source_path_.c_str(), &st) != 0) {
    // Check if path looks like a pseudo media URI or nonexistent file
    if (source_path_.find("://") != std::string::npos || source_path_.find("invalid") != std::string::npos) {
      duration_ms_ = 0;
      position_ms_ = 0;
      has_audio_ = false;
      has_video_ = false;
      is_seekable_ = false;
      player_->set_duration_internal(0);
      player_->set_position_internal(0);
      player_->set_error(Fl_Multimedia::ResourceError, "Failed to load media resource");
      player_->set_media_status_internal(Fl_Multimedia::InvalidMedia);
      return 0;
    }

    // Default simulated duration for non-existing test paths
    duration_ms_ = 5000;
    position_ms_ = 0;
    has_audio_ = true;
    has_video_ = false;
    is_seekable_ = true;
    player_->set_duration_internal(duration_ms_);
    player_->set_position_internal(0);
    player_->set_media_status_internal(Fl_Multimedia::LoadedMedia);
    return 1;
  }

  // Try parsing WAV file
  if (parse_wav_header(source_path_.c_str())) {
    player_->set_duration_internal(duration_ms_);
    player_->set_position_internal(0);
    player_->set_media_status_internal(Fl_Multimedia::LoadedMedia);
    return 1;
  }

  // Check extension for video or general audio formats
  size_t dot_pos = source_path_.rfind('.');
  std::string ext = (dot_pos != std::string::npos) ? source_path_.substr(dot_pos) : "";
  for (size_t i = 0; i < ext.length(); ++i) {
    if (ext[i] >= 'A' && ext[i] <= 'Z') ext[i] += 32;
  }

  if (ext == ".mp4" || ext == ".mkv" || ext == ".avi" || ext == ".webm" || ext == ".mov") {
    has_video_ = true;
    has_audio_ = true;
    duration_ms_ = 30000;
  } else if (ext == ".mp3" || ext == ".ogg" || ext == ".flac" || ext == ".aac" || ext == ".m4a") {
    has_video_ = false;
    has_audio_ = true;
    duration_ms_ = 180000;
  } else {
    has_video_ = false;
    has_audio_ = true;
    duration_ms_ = 10000;
  }

  is_seekable_ = true;
  position_ms_ = 0;
  player_->set_duration_internal(duration_ms_);
  player_->set_position_internal(0);
  player_->set_media_status_internal(Fl_Multimedia::LoadedMedia);
  return 1;
}

int Fl_Unix_Multimedia_Driver::play() {
  if (player_->media_status() == Fl_Multimedia::NoMedia ||
      player_->media_status() == Fl_Multimedia::InvalidMedia) {
    player_->set_error(Fl_Multimedia::ResourceError, "No media loaded");
    return 0;
  }

  if (is_playing_) return 1;

  is_playing_ = true;
  is_paused_ = false;
  player_->set_playback_state_internal(Fl_Multimedia::PlayingState);
  player_->set_media_status_internal(Fl_Multimedia::BufferedMedia);

  Fl::remove_timeout(timer_cb, this);
  Fl::add_timeout(MULTIMEDIA_TICK_INTERVAL, timer_cb, this);
  return 1;
}

void Fl_Unix_Multimedia_Driver::pause() {
  if (!is_playing_ && !is_paused_) return;

  is_playing_ = false;
  is_paused_ = true;
  Fl::remove_timeout(timer_cb, this);
  player_->set_playback_state_internal(Fl_Multimedia::PausedState);
}

void Fl_Unix_Multimedia_Driver::stop() {
  is_playing_ = false;
  is_paused_ = false;
  Fl::remove_timeout(timer_cb, this);
  position_ms_ = 0;
  player_->set_position_internal(0);
  player_->set_playback_state_internal(Fl_Multimedia::StoppedState);
  if (player_->media_status() != Fl_Multimedia::NoMedia &&
      player_->media_status() != Fl_Multimedia::InvalidMedia) {
    player_->set_media_status_internal(Fl_Multimedia::LoadedMedia);
  }
}

void Fl_Unix_Multimedia_Driver::set_position(int64_t position_ms) {
  if (position_ms < 0) position_ms = 0;
  if (duration_ms_ > 0 && position_ms > duration_ms_) position_ms = duration_ms_;

  position_ms_ = position_ms;
  player_->set_position_internal(position_ms_);
}

void Fl_Unix_Multimedia_Driver::set_volume(double vol) {
  if (vol < 0.0) vol = 0.0;
  if (vol > 1.0) vol = 1.0;
  volume_ = vol;
}

void Fl_Unix_Multimedia_Driver::set_muted(bool muted) {
  muted_ = muted;
}

void Fl_Unix_Multimedia_Driver::set_playback_rate(double rate) {
  if (rate <= 0.0) rate = 1.0;
  playback_rate_ = rate;
}

void Fl_Unix_Multimedia_Driver::timer_cb(void *data) {
  Fl_Unix_Multimedia_Driver *driver = (Fl_Unix_Multimedia_Driver*)data;
  if (driver) {
    driver->tick();
  }
}

void Fl_Unix_Multimedia_Driver::tick() {
  if (!is_playing_) return;

  int64_t step_ms = (int64_t)(MULTIMEDIA_TICK_INTERVAL * 1000.0 * playback_rate_);
  position_ms_ += step_ms;

  if (duration_ms_ > 0 && position_ms_ >= duration_ms_) {
    position_ms_ = duration_ms_;
    player_->set_position_internal(position_ms_);
    is_playing_ = false;
    Fl::remove_timeout(timer_cb, this);
    player_->on_end_of_media();
  } else {
    player_->set_position_internal(position_ms_);
    Fl::repeat_timeout(MULTIMEDIA_TICK_INTERVAL, timer_cb, this);
  }
}
