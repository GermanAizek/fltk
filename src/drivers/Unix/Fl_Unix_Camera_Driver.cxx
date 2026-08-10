//
// Fl_Unix_Camera_Driver implementation for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2025 by Bill Spitzak and others.
// Copyright 2026 by GermanAizek
//
#include "Fl_Unix_Camera_Driver.H"
#include <FL/Fl_Camera.H>
#include <FL/Fl_Image.H>
#include <FL/Fl.H>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#if defined(__linux__)
#include <linux/videodev2.h>
#define USE_V4L2 1
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
/* On BSD systems, webcamd/cuse provides V4L2 compatibility */
#if __has_include(<sys/videoio.h>)
#include <sys/videoio.h>
#elif __has_include(<linux/videodev2.h>)
#include <linux/videodev2.h>
#else
/* Fallback: attempt to include sys/videoio.h anyway if __has_include fails */
#include <sys/videoio.h>
#endif
#define USE_V4L2 1
#endif

Fl_Camera_Driver* Fl_Camera_Driver::new_camera_driver(Fl_Camera *widget) {
  return new Fl_Unix_Camera_Driver(widget);
}

Fl_Unix_Camera_Driver::Fl_Unix_Camera_Driver(Fl_Camera *widget)
  : Fl_Camera_Driver(widget), fd_(-1), thread_(0), running_(0), buffers_(0), n_buffers_(0) {
}

Fl_Unix_Camera_Driver::~Fl_Unix_Camera_Driver() {
  stop();
}

#if USE_V4L2

static int xioctl(int fh, unsigned long int request, void *arg) {
  int r;
  do {
    r = ioctl(fh, request, arg);
  } while (-1 == r && EINTR == errno);
  return r;
}

int Fl_Unix_Camera_Driver::init_device() {
  struct v4l2_capability cap;
  struct v4l2_format fmt;
  struct v4l2_requestbuffers req;

  if (-1 == xioctl(fd_, VIDIOC_QUERYCAP, &cap)) return 0;
  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) return 0;

  memset(&fmt, 0, sizeof(fmt));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width       = 640;
  fmt.fmt.pix.height      = 480;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

  if (-1 == xioctl(fd_, VIDIOC_S_FMT, &fmt)) return 0;

  memset(&req, 0, sizeof(req));
  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if (-1 == xioctl(fd_, VIDIOC_REQBUFS, &req)) return 0;

  buffers_ = (buffer*)calloc(req.count, sizeof(*buffers_));
  if (!buffers_) return 0;

  for (n_buffers_ = 0; n_buffers_ < req.count; ++n_buffers_) {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory      = V4L2_MEMORY_MMAP;
    buf.index       = n_buffers_;

    if (-1 == xioctl(fd_, VIDIOC_QUERYBUF, &buf)) return 0;

    buffers_[n_buffers_].length = buf.length;
    buffers_[n_buffers_].start =
      mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, buf.m.offset);

    if (MAP_FAILED == buffers_[n_buffers_].start) return 0;
  }
  return 1;
}

void Fl_Unix_Camera_Driver::uninit_device() {
  for (unsigned int i = 0; i < n_buffers_; ++i) {
    if (buffers_[i].start && buffers_[i].start != MAP_FAILED) {
      munmap(buffers_[i].start, buffers_[i].length);
    }
  }
  free(buffers_);
  buffers_ = 0;
  n_buffers_ = 0;
}

int Fl_Unix_Camera_Driver::start_capturing() {
  for (unsigned int i = 0; i < n_buffers_; ++i) {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    if (-1 == xioctl(fd_, VIDIOC_QBUF, &buf)) return 0;
  }
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == xioctl(fd_, VIDIOC_STREAMON, &type)) return 0;
  return 1;
}

void Fl_Unix_Camera_Driver::stop_capturing() {
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  xioctl(fd_, VIDIOC_STREAMOFF, &type);
}

static void yuyv_to_rgb(unsigned char *yuyv, unsigned char *rgb, int width, int height) {
  int z = 0;
  int x;
  int yline;

  for (yline = 0; yline < height; yline++) {
    for (x = 0; x < width; x += 2) {
      int y1 = yuyv[0];
      int u  = yuyv[1];
      int y2 = yuyv[2];
      int v  = yuyv[3];
      yuyv += 4;

      int r = y1 + 1.402 * (v - 128);
      int g = y1 - 0.344 * (u - 128) - 0.714 * (v - 128);
      int b = y1 + 1.772 * (u - 128);
      rgb[z++] = r < 0 ? 0 : (r > 255 ? 255 : r);
      rgb[z++] = g < 0 ? 0 : (g > 255 ? 255 : g);
      rgb[z++] = b < 0 ? 0 : (b > 255 ? 255 : b);

      r = y2 + 1.402 * (v - 128);
      g = y2 - 0.344 * (u - 128) - 0.714 * (v - 128);
      b = y2 + 1.772 * (u - 128);
      rgb[z++] = r < 0 ? 0 : (r > 255 ? 255 : r);
      rgb[z++] = g < 0 ? 0 : (g > 255 ? 255 : g);
      rgb[z++] = b < 0 ? 0 : (b > 255 ? 255 : b);
    }
  }
}

void Fl_Unix_Camera_Driver::process_frame() {
  struct v4l2_buffer buf;
  memset(&buf, 0, sizeof(buf));
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;

  if (-1 == xioctl(fd_, VIDIOC_DQBUF, &buf)) return;

  int width = 640;
  int height = 480;
  
  uchar *rgb = new uchar[width * height * 3];
  yuyv_to_rgb((unsigned char*)buffers_[buf.index].start, rgb, width, height);

  Fl::lock();
  if (frame_image_) {
    delete frame_image_;
  }
  frame_image_ = new Fl_RGB_Image(rgb, width, height, 3);
  frame_image_->alloc_array = 1;
  
  widget_->on_frame();
  Fl::unlock();

  xioctl(fd_, VIDIOC_QBUF, &buf);
}

void* Fl_Unix_Camera_Driver::capture_thread(void* arg) {
  Fl_Unix_Camera_Driver* self = (Fl_Unix_Camera_Driver*)arg;
  while (self->running_) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(self->fd_, &fds);
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    int r = select(self->fd_ + 1, &fds, NULL, NULL, &tv);
    if (r == -1) {
      if (EINTR == errno) continue;
      break;
    }
    if (r == 0) continue;

    self->process_frame();
  }
  return 0;
}

int Fl_Unix_Camera_Driver::start() {
  if (running_) return 1;

  fd_ = open("/dev/video0", O_RDWR | O_NONBLOCK, 0);
  if (fd_ == -1) {
    return 0;
  }

  if (!init_device()) {
    close(fd_);
    return 0;
  }

  if (!start_capturing()) {
    uninit_device();
    close(fd_);
    return 0;
  }

  running_ = 1;
  pthread_create(&thread_, NULL, capture_thread, this);
  return 1;
}

void Fl_Unix_Camera_Driver::stop() {
  if (!running_) return;
  running_ = 0;
  pthread_join(thread_, NULL);
  
  stop_capturing();
  uninit_device();
  close(fd_);
  fd_ = -1;
}

Fl_RGB_Image* Fl_Unix_Camera_Driver::get_frame() {
  return frame_image_;
}

#else // !USE_V4L2

int Fl_Unix_Camera_Driver::init_device() { return 0; }
void Fl_Unix_Camera_Driver::uninit_device() {}
int Fl_Unix_Camera_Driver::start_capturing() { return 0; }
void Fl_Unix_Camera_Driver::stop_capturing() {}
void Fl_Unix_Camera_Driver::process_frame() {}
void* Fl_Unix_Camera_Driver::capture_thread(void*) { return 0; }
int Fl_Unix_Camera_Driver::start() { return 0; }
void Fl_Unix_Camera_Driver::stop() {}
Fl_RGB_Image* Fl_Unix_Camera_Driver::get_frame() { return 0; }

#endif // USE_V4L2
