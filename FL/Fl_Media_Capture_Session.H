//
// Fl_Media_Capture_Session class for the Fast Light Tool Kit (FLTK).
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

#ifndef Fl_Media_Capture_Session_H
#define Fl_Media_Capture_Session_H

#include "Fl_Export.H"
#include "Fl_Camera.H"
#include "Fl_Audio_Input.H"
#include "Fl_Audio_Output.H"
#include "Fl_Video_Sink.H"
#include "Fl_Video_Widget.H"
#include "Fl_Image_Capture.H"
#include "Fl_Media_Recorder.H"
#include "Fl_Screen_Capture.H"
#include "Fl_Window_Capture.H"

/**
  \class Fl_Media_Capture_Session
  \brief Manages capture pipelines connecting inputs (camera, mic, screen, window) to outputs (video sink, image capture, recorder), matching Qt's QMediaCaptureSession.
*/
class FL_EXPORT Fl_Media_Capture_Session {
private:
  Fl_Camera *camera_;
  Fl_Audio_Input *audio_input_;
  Fl_Screen_Capture *screen_capture_;
  Fl_Window_Capture *window_capture_;
  Fl_Image_Capture *image_capture_;
  Fl_Media_Recorder *recorder_;
  Fl_Video_Sink *video_sink_;
  Fl_Video_Widget *video_output_;
  Fl_Audio_Output *audio_output_;

public:
  Fl_Media_Capture_Session();
  virtual ~Fl_Media_Capture_Session();

  /** Sets the camera input */
  void set_camera(Fl_Camera *camera);

  /** Returns current camera */
  Fl_Camera* camera() const { return camera_; }

  /** Sets the audio input (microphone) */
  void set_audio_input(Fl_Audio_Input *input);

  /** Returns current audio input */
  Fl_Audio_Input* audio_input() const { return audio_input_; }

  /** Sets the screen capture input */
  void set_screen_capture(Fl_Screen_Capture *screen);

  /** Returns current screen capture */
  Fl_Screen_Capture* screen_capture() const { return screen_capture_; }

  /** Sets the window capture input */
  void set_window_capture(Fl_Window_Capture *window);

  /** Returns current window capture */
  Fl_Window_Capture* window_capture() const { return window_capture_; }

  /** Sets the still image capture output */
  void set_image_capture(Fl_Image_Capture *image_capture);

  /** Returns current image capture */
  Fl_Image_Capture* image_capture() const { return image_capture_; }

  /** Sets the media recorder */
  void set_recorder(Fl_Media_Recorder *recorder);

  /** Returns current media recorder */
  Fl_Media_Recorder* recorder() const { return recorder_; }

  /** Sets the video sink */
  void set_video_sink(Fl_Video_Sink *sink);

  /** Returns current video sink */
  Fl_Video_Sink* video_sink() const { return video_sink_; }

  /** Sets the video output widget */
  void set_video_output(Fl_Video_Widget *widget);

  /** Returns current video output widget */
  Fl_Video_Widget* video_output() const { return video_output_; }

  /** Sets the audio output monitor */
  void set_audio_output(Fl_Audio_Output *output);

  /** Returns current audio output monitor */
  Fl_Audio_Output* audio_output() const { return audio_output_; }
};

#endif
