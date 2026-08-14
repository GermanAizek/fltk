//
// Fl_Video_Widget widget for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Video_Widget_H
#define Fl_Video_Widget_H

#include "Fl_Widget.H"
#include "Fl_Video_Sink.H"

/**
  \class Fl_Video_Widget
  \brief A widget to display video from a media player or capture session, matching Qt's QVideoWidget.
*/
class FL_EXPORT Fl_Video_Widget : public Fl_Widget {
public:
  enum AspectRatioMode {
    IgnoreAspectRatio            = 0,
    KeepAspectRatio              = 1,
    KeepAspectRatioByExpanding   = 2
  };

private:
  Fl_Video_Sink video_sink_;
  AspectRatioMode aspect_ratio_mode_;
  bool is_full_screen_;

  static void on_sink_frame(Fl_Video_Sink *sink, Fl_RGB_Image *frame, void *data);

protected:
  void draw() FL_OVERRIDE;

public:
  Fl_Video_Widget(int X, int Y, int W, int H, const char *L = 0);
  virtual ~Fl_Video_Widget();

  /** Returns the video sink associated with this video widget */
  Fl_Video_Sink* video_sink() { return &video_sink_; }

  /** Sets the aspect ratio scaling mode */
  void set_aspect_ratio_mode(AspectRatioMode mode);

  /** Returns the current aspect ratio mode */
  AspectRatioMode aspect_ratio_mode() const { return aspect_ratio_mode_; }

  /** Sets full screen mode flag */
  void set_full_screen(bool full);

  /** Returns whether full screen mode is active */
  bool is_full_screen() const { return is_full_screen_; }
};

#endif
