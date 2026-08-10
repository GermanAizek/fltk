//
// Fl_Camera widget for the Fast Light Tool Kit (FLTK).
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


#include "Fl_WinAPI_Camera_Driver.H"
#include <FL/Fl_Camera.H>
#include <FL/Fl_Image.H>
#include <FL/Fl.H>

Fl_Camera_Driver* Fl_Camera_Driver::new_camera_driver(Fl_Camera *widget) {
  return new Fl_WinAPI_Camera_Driver(widget);
}

Fl_WinAPI_Camera_Driver::Fl_WinAPI_Camera_Driver(Fl_Camera *widget)
  : Fl_Camera_Driver(widget), reader_(0), thread_(0), running_(0), com_initialized_(false) {
}

Fl_WinAPI_Camera_Driver::~Fl_WinAPI_Camera_Driver() {
  stop();
}

DWORD WINAPI Fl_WinAPI_Camera_Driver::capture_thread(LPVOID arg) {
  Fl_WinAPI_Camera_Driver* self = (Fl_WinAPI_Camera_Driver*)arg;
  while (self->running_) {
    self->process_frame();
  }
  return 0;
}

void Fl_WinAPI_Camera_Driver::process_frame() {
  if (!reader_) return;

  IMFSample *sample = NULL;
  DWORD streamIndex, flags;
  LONGLONG llTimeStamp;

  HRESULT hr = reader_->ReadSample(
    (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
    0,
    &streamIndex,
    &flags,
    &llTimeStamp,
    &sample
  );

  if (FAILED(hr)) {
    Sleep(10);
    return;
  }

  if (sample) {
    IMFMediaBuffer *buffer = NULL;
    hr = sample->ConvertToContiguousBuffer(&buffer);
    if (SUCCEEDED(hr)) {
      BYTE *data = NULL;
      DWORD maxLength, currentLength;
      hr = buffer->Lock(&data, &maxLength, &currentLength);
      if (SUCCEEDED(hr)) {
        int width = 640;
        int height = 480;
        
        IMFMediaType *mediaType = NULL;
        reader_->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &mediaType);
        if (mediaType) {
          MFGetAttributeSize(mediaType, MF_MT_FRAME_SIZE, (UINT32*)&width, (UINT32*)&height);
          mediaType->Release();
        }

        uchar *rgb = new uchar[width * height * 3];
        int z = 0;
        for (int y = height - 1; y >= 0; y--) { 
          int row_start = y * width * 4;
          for (int x = 0; x < width; x++) {
            rgb[z++] = data[row_start + x * 4 + 2]; // R
            rgb[z++] = data[row_start + x * 4 + 1]; // G
            rgb[z++] = data[row_start + x * 4 + 0]; // B
          }
        }

        buffer->Unlock();

        Fl::lock();
        if (frame_image_) delete frame_image_;
        frame_image_ = new Fl_RGB_Image(rgb, width, height, 3);
        frame_image_->alloc_array = 1;
        widget_->on_frame();
        Fl::unlock();
      }
      buffer->Release();
    }
    sample->Release();
  } else {
    Sleep(10);
  }
}

int Fl_WinAPI_Camera_Driver::start() {
  if (running_) return 1;

  HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
  if (SUCCEEDED(hr)) {
    com_initialized_ = true;
  }

  hr = MFStartup(MF_VERSION);
  if (FAILED(hr)) return 0;

  IMFAttributes *attributes = NULL;
  hr = MFCreateAttributes(&attributes, 1);
  if (FAILED(hr)) {
    MFShutdown();
    return 0;
  }

  hr = attributes->SetGUID(
    MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
    MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
  );
  if (FAILED(hr)) {
    attributes->Release();
    MFShutdown();
    return 0;
  }

  IMFActivate **devices = NULL;
  UINT32 count = 0;
  hr = MFEnumDeviceSources(attributes, &devices, &count);
  attributes->Release();

  if (FAILED(hr) || count == 0) {
    if (devices) CoTaskMemFree(devices);
    MFShutdown();
    return 0;
  }

  IMFMediaSource *source = NULL;
  hr = devices[0]->ActivateObject(IID_PPV_ARGS(&source));
  
  for (UINT32 i = 0; i < count; i++) {
    devices[i]->Release();
  }
  CoTaskMemFree(devices);

  if (FAILED(hr)) {
    MFShutdown();
    return 0;
  }

  hr = MFCreateSourceReaderFromMediaSource(source, NULL, &reader_);
  source->Release();

  if (FAILED(hr)) {
    MFShutdown();
    return 0;
  }

  IMFMediaType *type = NULL;
  hr = MFCreateMediaType(&type);
  if (SUCCEEDED(hr)) {
    type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    type->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
    reader_->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, type);
    type->Release();
  }

  running_ = 1;
  thread_ = CreateThread(NULL, 0, capture_thread, this, 0, NULL);
  return 1;
}

void Fl_WinAPI_Camera_Driver::stop() {
  if (!running_) return;
  running_ = 0;
  if (thread_) {
    WaitForSingleObject(thread_, INFINITE);
    CloseHandle(thread_);
    thread_ = 0;
  }
  if (reader_) {
    reader_->Release();
    reader_ = 0;
  }
  MFShutdown();
  if (com_initialized_) {
    CoUninitialize();
    com_initialized_ = false;
  }
}

Fl_RGB_Image* Fl_WinAPI_Camera_Driver::get_frame() {
  return frame_image_;
}
