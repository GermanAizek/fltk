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


#include "Fl_Cocoa_Camera_Driver.H"
#include <FL/Fl_Camera.H>
#include <FL/Fl_Image.H>
#include <FL/Fl.H>

#import <AVFoundation/AVFoundation.h>

@interface FlCameraDelegate : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate> {
  Fl_Cocoa_Camera_Driver *driver;
}
- (instancetype)initWithDriver:(Fl_Cocoa_Camera_Driver*)d;
@end

@implementation FlCameraDelegate
- (instancetype)initWithDriver:(Fl_Cocoa_Camera_Driver*)d {
  self = [super init];
  if (self) {
    driver = d;
  }
  return self;
}
- (void)captureOutput:(AVCaptureOutput *)output didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection {
  driver->process_frame(sampleBuffer);
}
@end

class Fl_Cocoa_Camera_Driver_Private {
public:
  AVCaptureSession *session;
  FlCameraDelegate *delegate;
  dispatch_queue_t queue;
};

Fl_Camera_Driver* Fl_Camera_Driver::new_camera_driver(Fl_Camera *widget) {
  return new Fl_Cocoa_Camera_Driver(widget);
}

Fl_Cocoa_Camera_Driver::Fl_Cocoa_Camera_Driver(Fl_Camera *widget)
  : Fl_Camera_Driver(widget), p(new Fl_Cocoa_Camera_Driver_Private), running_(0) {
  p->session = nil;
  p->delegate = nil;
}

Fl_Cocoa_Camera_Driver::~Fl_Cocoa_Camera_Driver() {
  stop();
  delete p;
}

int Fl_Cocoa_Camera_Driver::start() {
  if (running_) return 1;

  p->session = [[AVCaptureSession alloc] init];
  [p->session setSessionPreset:AVCaptureSessionPreset640x480];

  AVCaptureDevice *device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
  if (!device) return 0;

  NSError *error = nil;
  AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:device error:&error];
  if (!input) return 0;

  if ([p->session canAddInput:input]) {
    [p->session addInput:input];
  } else {
    return 0;
  }

  AVCaptureVideoDataOutput *output = [[AVCaptureVideoDataOutput alloc] init];
  output.videoSettings = @{ (id)kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_32BGRA) };
  output.alwaysDiscardsLateVideoFrames = YES;

  p->queue = dispatch_queue_create("fltk.camera.queue", NULL);
  p->delegate = [[FlCameraDelegate alloc] initWithDriver:this];
  [output setSampleBufferDelegate:p->delegate queue:p->queue];

  if ([p->session canAddOutput:output]) {
    [p->session addOutput:output];
  } else {
    return 0;
  }

  [p->session startRunning];
  running_ = 1;
  return 1;
}

void Fl_Cocoa_Camera_Driver::stop() {
  if (!running_) return;
  [p->session stopRunning];
  
  // Cleanup
  p->session = nil;
  p->delegate = nil;
  running_ = 0;
}

void Fl_Cocoa_Camera_Driver::process_frame(void* sampleBufferPtr) {
  CMSampleBufferRef sampleBuffer = (CMSampleBufferRef)sampleBufferPtr;
  CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
  
  CVPixelBufferLockBaseAddress(imageBuffer, kCVPixelBufferLock_ReadOnly);
  
  void *baseAddress = CVPixelBufferGetBaseAddress(imageBuffer);
  size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
  size_t width = CVPixelBufferGetWidth(imageBuffer);
  size_t height = CVPixelBufferGetHeight(imageBuffer);
  
  uchar *rgb = new uchar[width * height * 3];
  uchar *src = (uchar*)baseAddress;
  
  int z = 0;
  for (size_t y = 0; y < height; y++) {
    size_t row_start = y * bytesPerRow;
    for (size_t x = 0; x < width; x++) {
      rgb[z++] = src[row_start + x * 4 + 2]; // R
      rgb[z++] = src[row_start + x * 4 + 1]; // G
      rgb[z++] = src[row_start + x * 4 + 0]; // B
    }
  }
  
  CVPixelBufferUnlockBaseAddress(imageBuffer, kCVPixelBufferLock_ReadOnly);
  
  Fl::lock();
  if (frame_image_) delete frame_image_;
  frame_image_ = new Fl_RGB_Image(rgb, width, height, 3);
  frame_image_->alloc_array = 1;
  widget_->on_frame();
  Fl::unlock();
}

Fl_RGB_Image* Fl_Cocoa_Camera_Driver::get_frame() {
  return frame_image_;
}
