//
// Image drawing code for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2026 by Bill Spitzak and others.
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

#include <config.h>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Image.H>
#include "flstring.h"

#include <cmath>
#include <cstdlib>
#include <cstdint>
//
// Base image class...
//

Fl_RGB_Scaling Fl_Image::RGB_scaling_ = FL_RGB_SCALING_NEAREST;

Fl_RGB_Scaling Fl_Image::scaling_algorithm_ = FL_RGB_SCALING_BILINEAR;

/**
 The constructor creates an empty image with the specified
 width, height, and depth. The width and height are in pixels.
 The depth is 0 for bitmaps, 1 for pixmap (colormap) images, and
 1 to 4 for color images.
 */
Fl_Image::Fl_Image(int W, int H, int D) :
  w_(W), h_(H), d_(D), ld_(0), count_(0), data_w_(W), data_h_(H), data_(nullptr)
{}

/**
  The destructor is a virtual method that frees all memory used
  by the image.
*/
Fl_Image::~Fl_Image() = default;

/**
  If the image has been cached for display, delete the cache
  data. This allows you to change the data used for the image and
  then redraw it without recreating an image object.
*/
void Fl_Image::uncache() {
  static auto *plugin = Fl_Device_Plugin::opengl_plugin();
  if (plugin != nullptr) {
    plugin->delete_image_texture(this);
  }
}

void Fl_Image::draw(int XP, int YP, int /*w*/, int /*h*/, int /*cx*/, int /*cy*/) {
  draw_empty(XP, YP);
}

/**
  The protected method draw_empty() draws a box with
  an X in it. It can be used to draw any image that lacks image
  data.
*/
void Fl_Image::draw_empty(int X, int Y) const {
  if (w() > 0 && h() > 0) {
    fl_color(FL_FOREGROUND_COLOR);
    fl_rect(X, Y, w(), h());
    fl_line(X, Y, X + w() - 1, Y + h() - 1);
    fl_line(X, Y + h() - 1, X + w() - 1, Y);
  }
}

/**
  Creates a resized copy of the image.

  It is recommended not to call this member function to reduce the size
  of an image to the size of the area where this image will be drawn,
  and to use Fl_Image::scale() instead.

  The new image should be released when you are done with it.

  Note: since FLTK 1.4.0 you can use Fl_Image::release() for all types
  of images (i.e. all subclasses of Fl_Image) instead of operator \em delete
  for Fl_Image's and Fl_Image::release() for Fl_Shared_Image's.

  The new image data will be converted to the requested size. RGB images
  are resized using the algorithm set by Fl_Image::RGB_scaling().

  For the new image the following equations are true:
  - w() == data_w() == \p W
  - h() == data_h() == \p H

 \param[in] W,H  Requested width and height of the new image

  \note The returned image can be safely cast to the same image type as that
  of the source image provided this type is one of Fl_RGB_Image, Fl_SVG_Image,
  Fl_Pixmap, Fl_Bitmap, Fl_Tiled_Image,  Fl_Anim_GIF_Image and Fl_Shared_Image.
  Returned objects copied from images of other, derived, image classes belong
  to the parent class appearing in this list. For example, the copy of an
  Fl_GIF_Image is an object of class Fl_Pixmap.

  \note Since FLTK 1.4.0 this method is 'const'. If you derive your own class
    from Fl_Image or any subclass your overridden methods of 'Fl_Image::copy() const'
    and 'Fl_Image::copy(int, int) const' \b must also be 'const' for inheritance
    to work properly. This is different than in FLTK 1.3.x and earlier where these
    methods have not been 'const'.
*/
Fl_Image *Fl_Image::copy(int W, int H) const {
  return new Fl_Image(W, H, d());
}

/**
  The color_average() method averages the colors in the image with
  the provided FLTK color value.

  The first argument specifies the FLTK color to be used.

  The second argument specifies the amount of the original image to combine
  with the color, so a value of 1.0 results in no color blend, and a value
  of 0.0 results in a constant image of the specified color.

  An internal copy is made of the original image data before changes are
  applied, to avoid modifying the original image data in memory.
*/
void Fl_Image::color_average(Fl_Color /*color*/, float /*weight*/) {
}

/**
  The desaturate() method converts an image to grayscale.

  If the image contains an alpha channel (depth = 4),
  the alpha channel is preserved.

  An internal copy is made of the original image data before changes are
  applied, to avoid modifying the original image data in memory.
*/
void Fl_Image::desaturate() {
}

// Doxygen documentation in FL/Enumerations.H
Fl_Labeltype fl_define_FL_IMAGE_LABEL() {
  return Fl_Image::define_FL_IMAGE_LABEL();
}

Fl_Labeltype Fl_Image::define_FL_IMAGE_LABEL() {
  Fl::set_labeltype(_FL_IMAGE_LABEL, Fl_Image::labeltype, Fl_Image::measure);
  return _FL_IMAGE_LABEL;
}

/**
  This method is an obsolete way to set the image attribute of a widget
  or menu item.

  \deprecated Please use Fl_Widget::image() or Fl_Widget::deimage() instead.
*/
void Fl_Image::label(Fl_Widget* widget) {
  widget->image(this);
}

/**
  This method is an obsolete way to set the image attribute of a menu item.

  \deprecated Please use Fl_Menu_Item::image() instead.
*/
void Fl_Image::label(Fl_Menu_Item* m) {
  m->label(FL_IMAGE_LABEL, reinterpret_cast<const char*>(this));
}

/**
  Returns a value that is not 0 if there is currently no image available.

  Example use:
  \code
    // [..]
      Fl_Box box(X, Y, W, H);
      Fl_JPEG_Image jpg("/tmp/foo.jpg");
      switch (jpg.fail()) {
        case Fl_Image::ERR_NO_IMAGE:
        case Fl_Image::ERR_FILE_ACCESS:
          fl_alert("/tmp/foo.jpg: %s", strerror(errno));    // shows actual os error to user
          exit(1);
        case Fl_Image::ERR_FORMAT:
          fl_alert("/tmp/foo.jpg: couldn't decode image");
          exit(1);
      }
      box.image(jpg);
  \endcode

  \returns                  Image load failure if non-zero
  \retval 0                 the image was loaded successfully
  \retval ERR_NO_IMAGE      no image was found
  \retval ERR_FILE_ACCESS   there was a file access related error (errno should be set)
  \retval ERR_FORMAT        image decoding failed
  \retval ERR_MEMORY_ACCESS image decoder tried to access memory outside of given memory block
*/
int Fl_Image::fail() const {
  if ((w_ <= 0) || (h_ <= 0) || (d_ <= 0 && count_ == 0)) {
    if (ld_ == 0) {
      return ERR_NO_IMAGE;
    }
    return ld_;
  }
  return 0;
}

void
Fl_Image::labeltype(const Fl_Label *lo,
                    int            lx,
                    int            ly,
                    int            lw,
                    int            lh,
                    Fl_Align       la) {
  auto *img = const_cast<Fl_Image *>(reinterpret_cast<const Fl_Image *>(lo->value));
  if (!img) return;

  int cx = 0;
  int cy = 0;

  if ((la & FL_ALIGN_LEFT) != 0) {
    cx = 0;
  } else if ((la & FL_ALIGN_RIGHT) != 0) {
    cx = img->w() - lw;
  } else {
    cx = (img->w() - lw) / 2;
  }

  if ((la & FL_ALIGN_TOP) != 0) {
    cy = 0;
  } else if ((la & FL_ALIGN_BOTTOM) != 0) {
    cy = img->h() - lh;
  } else {
    cy = (img->h() - lh) / 2;
  }

  fl_color(lo->color);

  img->draw(lx, ly, lw, lh, cx, cy);
}

void
Fl_Image::measure(const Fl_Label *lo,
                  int            &lw,
                  int            &lh) {
  const auto *img = (const Fl_Image *)(lo->value);

  lw = img->w();
  lh = img->h();
}

/** Sets the RGB image scaling method used for copy(int, int).
    Applies to all RGB images, defaults to FL_RGB_SCALING_NEAREST.
*/
void Fl_Image::RGB_scaling(Fl_RGB_Scaling method) {
  RGB_scaling_ = method;
}

/** Returns the currently used RGB image scaling method. */
Fl_RGB_Scaling Fl_Image::RGB_scaling() {
  return RGB_scaling_;
}

/** Sets the drawing size of the image.
 This function controls the values returned by member functions w() and h()
 which in turn control how the image is drawn: the full image data (whose size
 is given by data_w() and data_h()) are drawn scaled
 to an area of the drawing surface sized at w() x h() FLTK units.
 This can make a difference if the drawing surface has more than 1 pixel per
 FLTK unit because the image can be drawn at the full resolution of the drawing surface.
 Examples of such drawing surfaces: HiDPI displays, laser printers, PostScript files, PDF printers.

 \param width,height   maximum values, in FLTK units, that w() and h() should return
 \param proportional   if not null, keep the values returned by w() and h() proportional to
 data_w() and data_h()
 \param can_expand  if null, the values returned by w() and h() will not be larger than
 data_w() and data_h(), respectively
 \note This function generally changes the values returned by the w() and h() member functions.
 In contrast, the values returned by data_w() and data_h() remain unchanged.
 \note If the processed image is an Fl_SVG_Image, setting \ref Fl_SVG_Image::proportional
 to \c false is required if \p proportional here is null.
 \version 1.4 (1.3.4 and FL_ABI_VERSION for Fl_Shared_Image only)

 Example code: scale an image to fit in a box
 \code
 Fl_Box *b = ...  // a box
 Fl_Image *img = new Fl_PNG_Image("/path/to/picture.png"); // read a picture file
 // set the drawing size of the image to the size of the box keeping its aspect ratio
 img->scale(b->w(), b->h());
 b->image(img); // use the image as the box image
 \endcode
 */
void Fl_Image::scale(int width, int height, int proportional, int can_expand)
{
  if ((width <= data_w() && height <= data_h()) || can_expand != 0) {
    w_ = width;
    h_ = height;
  }
  if (fail() != 0) {
    return;
  }
  if (proportional == 0 && can_expand != 0) {
    return;
  }
  if (proportional == 0 && width <= data_w() && height <= data_h()) {
    return;
  }

  float fw = static_cast<float>(data_w()) / static_cast<float>(width);
  float fh = static_cast<float>(data_h()) / static_cast<float>(height);

  if (proportional != 0) {
    if (fh > fw) {
      fw = fh;
    } else {
      fh = fw;
    }
  }
  if (can_expand == 0) {
    if (fw < 1.0F) {
      fw = 1.0F;
    }
    if (fh < 1.0F) {
      fh = 1.0F;
    }
  }
  w_ = static_cast<int>(std::lround(static_cast<float>(data_w()) / fw));
  h_ = static_cast<int>(std::lround(static_cast<float>(data_h()) / fh));
}

int Fl_Image::draw_scaled(int X, int Y, int W, int H) {
  const int width = w();
  const int height = h();
  scale(W, H, 0, 1);
  draw(X, Y, W, H, 0, 0);
  scale(width, height, 0, 1);
  return 1;
}

/** True after fl_register_images() was called, false before */
bool Fl_Image::register_images_done = false;

//
// RGB image class...
//
size_t Fl_RGB_Image::max_size_ = ~static_cast<size_t>(0);

Fl_RGB_Image::Fl_RGB_Image(const uchar *bits, int W, int H, int D, int LD) :
  Fl_Image(W, H, D),
  array(bits),
  alloc_array(0),
  id_(0),
  mask_(0),
  cache_w_(0), cache_h_(0)
{
    data((const char **)&array, 1);
    ld(LD);
}


/**
 The constructor creates a new image from the specified data.

 If the provided array is too small to contain all the image data, the
 constructor will not generate the image to avoid illegal memory read
 access and instead set \c data to NULL and \c ld to \c ERR_MEMORY_ACCESS.

 \param bits image data
 \param bits_length length of the \p bits array in bytes
 \param W image width in pixels
 \param H image height in pixels
 \param D image depth in bytes, 1 for gray scale, 2 for gray with alpha,
        3 for RGB, and 4 for RGB plus alpha
 \param LD line length in bytes, or 0 to use W*D.

 \see Fl_RGB_Image(const uchar *bits, int W, int H, int D, int LD)
 */
Fl_RGB_Image::Fl_RGB_Image(const uchar *bits, int bits_length, int W, int H, int D, int LD) :
  Fl_Image(W, H, D),
  array(bits),
  alloc_array(0),
  id_(0),
  mask_(0),
  cache_w_(0), cache_h_(0)
{
  if (D == 0) {
    D = 3;
  }
  if (LD == 0) {
    LD = W * D;
  }
  const int min_length = LD * (H - 1) + W * D;
  if (bits_length >= min_length) {
    data((const char **)&array, 1);
    ld(LD);
  } else {
    array = nullptr;
    data(nullptr, 0);
    ld(ERR_MEMORY_ACCESS);
  }
}

int fl_convert_pixmap(const char* const* cdata, uchar* out, Fl_Color bg);

/**
  The constructor creates a new RGBA image from the specified Fl_Pixmap.

  The RGBA image is built fully opaque except for the transparent area
  of the pixmap that is assigned the \p bg color with full transparency.

  This constructor creates a new internal data array and sets
  Fl_RGB_Image::alloc_array to 1 so the data array is deleted when the
  image is destroyed.
*/
Fl_RGB_Image::Fl_RGB_Image(const Fl_Pixmap *pxm, const Fl_Color bg):
  Fl_Image(pxm->data_w(), pxm->data_h(), 4),
  array(nullptr),
  alloc_array(0),
  id_(0),
  mask_(0),
  cache_w_(0), cache_h_(0)
{
  if (pxm != nullptr && pxm->data_w() > 0 && pxm->data_h() > 0) {
    array = new uchar[static_cast<size_t>(data_w()) * data_h() * d()];
    alloc_array = 1;
    fl_convert_pixmap(pxm->data(), const_cast<uchar*>(array), bg);
  }
  data((const char **)&array, 1);
  scale(pxm->w(), pxm->h(), 0, 1);
}


/**
  The destructor frees all memory and server resources that are used by
  the image.
*/
Fl_RGB_Image::~Fl_RGB_Image() {
  Fl_RGB_Image::uncache();
  if (alloc_array != 0) {
    delete[] array;
  }
}

void Fl_RGB_Image::uncache() {
  Fl_Graphics_Driver::default_driver().uncache(this, id_, mask_);
  Fl_Image::uncache();
}

/**
 Optimize the simple copy where the width and height are the same,
 or when we are copying an empty image.
 */
Fl_RGB_Image *Fl_RGB_Image::copy_optimize_(int W, int H) const {
  if (array != nullptr) {
    const size_t total_size = static_cast<size_t>(W) * H * d();
    auto *new_array = new uchar[total_size];
    if (ld() != 0 && (ld() != W * d())) {
      const uchar *src = array;
      uchar *dst = new_array;
      const int dh = H;
      const int wd = W * d();
      const int wld = ld();
      for (int dy = 0; dy < dh; dy++) {
        memcpy(dst, src, wd);
        src += wld;
        dst += wd;
      }
    } else {
      memcpy(new_array, array, total_size);
    }
    auto *new_image = new Fl_RGB_Image(new_array, W, H, d());
    new_image->alloc_array = 1;
    return new_image;
  }

  return new Fl_RGB_Image(array, W, H, d(), ld());
}

/**
 Create a scaled up or down copy of this image using nearest neighbor.
 */
Fl_RGB_Image *Fl_RGB_Image::copy_nearest_neighbor_(int W, int H) const {
  auto *new_array = new uchar[static_cast<size_t>(W) * H * d()];
  auto *new_image = new Fl_RGB_Image(new_array, W, H, d());
  new_image->alloc_array = 1;

  const int line_d = ld() != 0 ? ld() : data_w() * d();

  const int xmod = data_w() % W;
  const int ymod = data_h() % H;
  const int ystep = data_h() / H;

  auto *x_offset = new int[W];
  for (int dx = 0, err = W, current_x = 0; dx < W; dx++) {
    x_offset[dx] = current_x * d();
    current_x += (data_w() / W);
    err -= xmod;
    if (err <= 0) {
      err += W;
      current_x++;
    }
  }

  uchar *new_ptr = new_array;
  for (int dy = H, sy = 0, yerr = H; dy > 0; dy--) {
    const uchar* line_ptr = array + static_cast<size_t>(sy) * line_d;
    switch (d()) {
      case 1:
        for (int dx = 0; dx < W; dx++) {
          *new_ptr++ = line_ptr[x_offset[dx]];
        }
        break;
      case 2:
        for (int dx = 0; dx < W; dx++) {
          const uchar* old_ptr = line_ptr + x_offset[dx];
          *new_ptr++ = old_ptr[0];
          *new_ptr++ = old_ptr[1];
        }
        break;
      case 3:
        for (int dx = 0; dx < W; dx++) {
          const uchar* old_ptr = line_ptr + x_offset[dx];
          *new_ptr++ = old_ptr[0];
          *new_ptr++ = old_ptr[1];
          *new_ptr++ = old_ptr[2];
        }
        break;
      case 4:
        for (int dx = 0; dx < W; dx++) {
          const uchar* old_ptr = line_ptr + x_offset[dx];
          *new_ptr++ = old_ptr[0];
          *new_ptr++ = old_ptr[1];
          *new_ptr++ = old_ptr[2];
          *new_ptr++ = old_ptr[3];
        }
        break;
      default:
        for (int dx = 0; dx < W; dx++) {
          const uchar* old_ptr = line_ptr + x_offset[dx];
          for (int c = 0; c < d(); c++) {
            *new_ptr++ = old_ptr[c];
          }
        }
        break;
    }
    sy += ystep;
    yerr -= ymod;
    if (yerr <= 0) {
      yerr += H;
      sy++;
    }
  }

  delete[] x_offset;
  return new_image;
}

/**
  Create a scaled up or down copy of this image using bilinear interpolation.

  Scaling quality is best for factors > 0.5. Scaling below 0.5 can be achieved
  by using copy_scale_down...() and copy_scale_up() first until the resulting
  scale factor is greater than 0.5 .

  This function can scale up or down for w and h independently.

  RGB or gray must not be premultiplied if alpha is used.

  Yes, this function is relatively slow. It may be worth looking to host OS
  support for scaling images.

  \param[in] W, H  Requested width and height of the new image
  \returns  A new image object with the requested size. The caller is responsible
           for deleting the returned image object when it is no longer needed.
*/
Fl_RGB_Image *Fl_RGB_Image::copy_bilinear_(uint32_t W, uint32_t H) const {
  const auto D = static_cast<uint32_t>(d());
  const auto SW = static_cast<uint32_t>(data_w());
  const auto SH = static_cast<uint32_t>(data_h());
  const uint32_t SLD = ld() != 0 ? static_cast<uint32_t>(ld()) : SW * D;

  auto *new_array = new uint8_t[static_cast<size_t>(W) * H * D];
  auto *new_image = new Fl_RGB_Image(new_array, static_cast<int>(W), static_cast<int>(H), static_cast<int>(D));
  new_image->alloc_array = 1;

  auto *x0_off = new uint32_t[W];
  auto *x1_off = new uint32_t[W];
  auto *wx1 = new uint32_t[W];

  auto *y0_off = new uint32_t[H];
  auto *y1_off = new uint32_t[H];
  auto *wy1 = new uint32_t[H];

  for (uint32_t x = 0; x < W; ++x) {
    const float sx = ((static_cast<float>(x) + 0.5F) * static_cast<float>(SW)) / static_cast<float>(W) - 0.5F;
    auto x0 = static_cast<int32_t>(sx);
    if (sx < 0.0F && static_cast<float>(x0) != sx) {
      x0--;
    }

    float fx = sx - static_cast<float>(x0);

    if (x0 < 0) {
      x0 = 0;
      fx = 0.0F;
    } else if (x0 >= static_cast<int32_t>(SW) - 1) {
      x0 = static_cast<int32_t>(SW) - 1;
      fx = 0.0F;
    }

    const uint32_t x1 = (x0 < static_cast<int32_t>(SW) - 1) ? static_cast<uint32_t>(x0 + 1) : static_cast<uint32_t>(x0);
    auto w = static_cast<int32_t>(std::lround(fx * 256.0F));
    if (w < 0) {
      w = 0;
    } else if (w > 256) {
      w = 256;
    }

    x0_off[x] = static_cast<uint32_t>(x0) * D;
    x1_off[x] = x1 * D;
    wx1[x] = static_cast<uint32_t>(w);
  }

  for (uint32_t y = 0; y < H; ++y) {
    const float sy = ((static_cast<float>(y) + 0.5F) * static_cast<float>(SH)) / static_cast<float>(H) - 0.5F;
    auto y0 = static_cast<int32_t>(sy);
    if (sy < 0.0F && static_cast<float>(y0) != sy) {
      y0--;
    }

    float fy = sy - static_cast<float>(y0);

    if (y0 < 0) {
      y0 = 0;
      fy = 0.0F;
    } else if (y0 >= static_cast<int32_t>(SH) - 1) {
      y0 = static_cast<int32_t>(SH) - 1;
      fy = 0.0F;
    }

    const uint32_t y1 = (y0 < static_cast<int32_t>(SH) - 1) ? static_cast<uint32_t>(y0 + 1) : static_cast<uint32_t>(y0);
    auto w = static_cast<int32_t>(std::lround(fy * 256.0F));
    if (w < 0) {
      w = 0;
    } else if (w > 256) {
      w = 256;
    }

    y0_off[y] = static_cast<uint32_t>(y0) * SLD;
    y1_off[y] = y1 * SLD;
    wy1[y] = static_cast<uint32_t>(w);
  }

  for (uint32_t y = 0; y < H; ++y) {
    const uint8_t *row0 = array + y0_off[y];
    const uint8_t *row1 = array + y1_off[y];
    uint8_t *dst = new_array + y * W * D;

    const uint32_t wy = wy1[y];
    const uint32_t wy0 = 256U - wy;

    for (uint32_t x = 0; x < W; ++x) {
      const uint8_t *p00 = row0 + x0_off[x];
      const uint8_t *p10 = row0 + x1_off[x];
      const uint8_t *p01 = row1 + x0_off[x];
      const uint8_t *p11 = row1 + x1_off[x];

      const uint32_t wx = wx1[x];
      const uint32_t wx0 = 256U - wx;

      for (uint32_t c = 0; c < D; ++c) {
        const uint32_t top = static_cast<uint32_t>(p00[c]) * wx0 + static_cast<uint32_t>(p10[c]) * wx;
        const uint32_t bot = static_cast<uint32_t>(p01[c]) * wx0 + static_cast<uint32_t>(p11[c]) * wx;
        dst[c] = static_cast<uint8_t>((top * wy0 + bot * wy + 32768U) >> 16U);
      }
      dst += D;
    }
  }

  delete[] x0_off;
  delete[] x1_off;
  delete[] wx1;
  delete[] y0_off;
  delete[] y1_off;
  delete[] wy1;

  return new_image;
}

/**
  Create a scaled down copy of this image by a factor of 2 in the horizontal direction.
 */
Fl_RGB_Image *Fl_RGB_Image::copy_scale_down_2h_() const {
  const int W = data_w() / 2;
  const int H = data_h();
  const int D = d();
  const int LD = ld() != 0 ? ld() : data_w() * D;
  if ((W == 0) || (H == 0) || (D == 0)) {
    return nullptr;
  }

  auto *data = new uchar[static_cast<size_t>(W) * H * D];
  uchar *dst = data;
  for (int y = 0; y < H; y++) {
    const uchar *src = array + static_cast<size_t>(y) * LD;
    switch (D) {
      case 1:
        for (int x = 0; x < W; ++x) {
          dst[x] = static_cast<uchar>((static_cast<unsigned>(src[x * 2 + 0]) + static_cast<unsigned>(src[x * 2 + 1])) >> 1U);
        }
        dst += W;
        break;
      case 2:
        for (int x = 0; x < W; ++x) {
          dst[x * 2 + 0] = static_cast<uchar>((static_cast<unsigned>(src[x * 4 + 0]) + static_cast<unsigned>(src[x * 4 + 2])) >> 1U);
          dst[x * 2 + 1] = static_cast<uchar>((static_cast<unsigned>(src[x * 4 + 1]) + static_cast<unsigned>(src[x * 4 + 3])) >> 1U);
        }
        dst += W * 2;
        break;
      case 3:
        for (int x = 0; x < W; ++x) {
          dst[x * 3 + 0] = static_cast<uchar>((static_cast<unsigned>(src[x * 3 * 2 + 0]) + static_cast<unsigned>(src[x * 3 * 2 + 3])) >> 1U);
          dst[x * 3 + 1] = static_cast<uchar>((static_cast<unsigned>(src[x * 3 * 2 + 1]) + static_cast<unsigned>(src[x * 3 * 2 + 4])) >> 1U);
          dst[x * 3 + 2] = static_cast<uchar>((static_cast<unsigned>(src[x * 3 * 2 + 2]) + static_cast<unsigned>(src[x * 3 * 2 + 5])) >> 1U);
        }
        dst += W * 3;
        break;
      case 4:
        for (int x = 0; x < W; ++x) {
          dst[x * 4 + 0] = static_cast<uchar>((static_cast<unsigned>(src[x * 4 * 2 + 0]) + static_cast<unsigned>(src[x * 4 * 2 + 4])) >> 1U);
          dst[x * 4 + 1] = static_cast<uchar>((static_cast<unsigned>(src[x * 4 * 2 + 1]) + static_cast<unsigned>(src[x * 4 * 2 + 5])) >> 1U);
          dst[x * 4 + 2] = static_cast<uchar>((static_cast<unsigned>(src[x * 4 * 2 + 2]) + static_cast<unsigned>(src[x * 4 * 2 + 6])) >> 1U);
          dst[x * 4 + 3] = static_cast<uchar>((static_cast<unsigned>(src[x * 4 * 2 + 3]) + static_cast<unsigned>(src[x * 4 * 2 + 7])) >> 1U);
        }
        dst += W * 4;
        break;
      default:
        break;
    }
  }
  auto *new_img = new Fl_RGB_Image(data, W, H, D);
  new_img->alloc_array = 1;
  return new_img;
}

Fl_RGB_Image *Fl_RGB_Image::copy_scale_down_2v_() const {
  const int W = data_w();
  const int H = data_h() / 2;
  const int D = d();
  const int LD = ld() != 0 ? ld() : data_w() * D;
  if ((W == 0) || (H == 0) || (D == 0)) {
    return nullptr;
  }

  auto *data = new uchar[static_cast<size_t>(W) * H * D];
  uchar *dst = data;
  for (int y = 0; y < H; y++) {
    const uchar *s0 = array + static_cast<size_t>(2 * y) * LD;
    const uchar *s1 = s0 + LD;
    switch (D) {
      case 1:
        for (int x = 0; x < W; ++x) {
          *dst++ = static_cast<uchar>((static_cast<unsigned>(*s0++) + static_cast<unsigned>(*s1++)) >> 1U);
        }
        break;
      case 2:
        for (int x = 0; x < W; ++x) {
          *dst++ = static_cast<uchar>((static_cast<unsigned>(*s0++) + static_cast<unsigned>(*s1++)) >> 1U);
          *dst++ = static_cast<uchar>((static_cast<unsigned>(*s0++) + static_cast<unsigned>(*s1++)) >> 1U);
        }
        break;
      case 3:
        for (int x = 0; x < W; ++x) {
          *dst++ = static_cast<uchar>((static_cast<unsigned>(*s0++) + static_cast<unsigned>(*s1++)) >> 1U);
          *dst++ = static_cast<uchar>((static_cast<unsigned>(*s0++) + static_cast<unsigned>(*s1++)) >> 1U);
          *dst++ = static_cast<uchar>((static_cast<unsigned>(*s0++) + static_cast<unsigned>(*s1++)) >> 1U);
        }
        break;
      case 4:
        for (int x = 0; x < W; ++x) {
          *dst++ = static_cast<uchar>((static_cast<unsigned>(*s0++) + static_cast<unsigned>(*s1++)) >> 1U);
          *dst++ = static_cast<uchar>((static_cast<unsigned>(*s0++) + static_cast<unsigned>(*s1++)) >> 1U);
          *dst++ = static_cast<uchar>((static_cast<unsigned>(*s0++) + static_cast<unsigned>(*s1++)) >> 1U);
          *dst++ = static_cast<uchar>((static_cast<unsigned>(*s0++) + static_cast<unsigned>(*s1++)) >> 1U);
        }
        break;
      default:
        break;
    }
  }
  auto *new_img = new Fl_RGB_Image(data, W, H, D);
  new_img->alloc_array = 1;
  return new_img;
}

Fl_Image *Fl_RGB_Image::copy(int W, int H) const {
  if ((W == data_w() && H == data_h()) || (w() == 0) || (h() == 0) || (d() == 0) || array == nullptr) {
    return copy_optimize_(W, H);
  }
  if (W <= 0 || H <= 0) {
    return nullptr;
  }
  if (Fl_Image::RGB_scaling() == FL_RGB_SCALING_NEAREST) {
    return copy_nearest_neighbor_(W, H);
  }

  const Fl_RGB_Image *img = this;
  while ((img->data_w() >= 2 * W) || (img->data_h() >= 2 * H)) {
    if (img->data_w() >= 2 * W) {
      const Fl_RGB_Image *scaled_img = img->copy_scale_down_2h_();
      if (img != this) {
        delete img;
      }
      img = scaled_img;
    }
    if (img->data_h() >= 2 * H) {
      const Fl_RGB_Image *scaled_img = img->copy_scale_down_2v_();
      if (img != this) {
        delete img;
      }
      img = scaled_img;
    }
  }

  if ((img->data_w() != W) || (img->data_h() != H)) {
    Fl_RGB_Image *fine_scaled_img = img->copy_bilinear_(static_cast<uint32_t>(W), static_cast<uint32_t>(H));
    if (img != this) {
      delete img;
    }
    return fine_scaled_img;
  }

  if (img == this) {
    return copy();
  }
  return const_cast<Fl_RGB_Image*>(img);
}

void Fl_RGB_Image::color_average(Fl_Color c, float i) {
  if (w() == 0 || h() == 0 || d() == 0 || array == nullptr) {
    return;
  }

  uncache();

  uchar *new_array = nullptr;
  if (alloc_array == 0) {
    new_array = new uchar[static_cast<size_t>(data_h()) * data_w() * d()];
  } else {
    new_array = const_cast<uchar*>(array);
  }

  uchar r = 0;
  uchar g = 0;
  uchar b = 0;
  Fl::get_color(c, r, g, b);

  if (i < 0.0F) {
    i = 0.0F;
  } else if (i > 1.0F) {
    i = 1.0F;
  }

  const auto ia = static_cast<unsigned>(256.0F * i);
  const unsigned ir = r * (256U - ia);
  unsigned ig = g * (256U - ia);
  const unsigned ib = b * (256U - ia);

  const uchar *old_ptr = array;
  uchar *new_ptr = new_array;
  const int line_i = ld() != 0 ? ld() - (data_w() * d()) : 0;

  if (d() < 3) {
    ig = (r * 31U + g * 61U + b * 8U) / 100U * (256U - ia);

    for (int y = 0; y < data_h(); y++, old_ptr += line_i) {
      for (int x = 0; x < data_w(); x++) {
        *new_ptr++ = static_cast<uchar>((static_cast<unsigned>(*old_ptr++) * ia + ig) >> 8U);
        if (d() > 1) {
          *new_ptr++ = *old_ptr++;
        }
      }
    }
  } else {
    for (int y = 0; y < data_h(); y++, old_ptr += line_i) {
      for (int x = 0; x < data_w(); x++) {
        *new_ptr++ = static_cast<uchar>((static_cast<unsigned>(*old_ptr++) * ia + ir) >> 8U);
        *new_ptr++ = static_cast<uchar>((static_cast<unsigned>(*old_ptr++) * ia + ig) >> 8U);
        *new_ptr++ = static_cast<uchar>((static_cast<unsigned>(*old_ptr++) * ia + ib) >> 8U);
        if (d() > 3) {
          *new_ptr++ = *old_ptr++;
        }
      }
    }
  }

  if (alloc_array == 0) {
    array = new_array;
    alloc_array = 1;
    ld(0);
  }
}

void Fl_RGB_Image::desaturate() {
  if (w() == 0 || h() == 0 || d() == 0 || array == nullptr) {
    return;
  }
  if (d() < 3) {
    return;
  }

  uncache();

  const int new_d = d() - 2;
  auto *new_array = new uchar[static_cast<size_t>(data_h()) * data_w() * new_d];

  const uchar *old_ptr = array;
  uchar *new_ptr = new_array;
  const int line_i = ld() != 0 ? ld() - (data_w() * d()) : 0;

  for (int y = 0; y < data_h(); y++, old_ptr += line_i) {
    for (int x = 0; x < data_w(); x++, old_ptr += d()) {
      *new_ptr++ = static_cast<uchar>((31U * static_cast<unsigned>(old_ptr[0]) +
                                       61U * static_cast<unsigned>(old_ptr[1]) +
                                       8U  * static_cast<unsigned>(old_ptr[2])) / 100U);
      if (d() > 3) {
        *new_ptr++ = old_ptr[3];
      }
    }
  }

  if (alloc_array != 0) {
    delete[] array;
  }

  array = new_array;
  alloc_array = 1;

  ld(0);
  d(new_d);
}

namespace {
  inline int fl_max(int a, int b) { return (a > b) ? a : b; }
  inline int fl_min(int a, int b) { return (a < b) ? a : b; }

  struct RectangleInt {
    int x;
    int y;
    int width;
    int height;
  };

  void crect_intersect(RectangleInt *to, const RectangleInt *with) {
    const int x = fl_max(to->x, with->x);
    to->width = fl_min(to->x + to->width, with->x + with->width) - x;
    if (to->width < 0) {
      to->width = 0;
    }
    const int y = fl_max(to->y, with->y);
    to->height = fl_min(to->y + to->height, with->y + with->height) - y;
    if (to->height < 0) {
      to->height = 0;
    }
    to->x = x;
    to->y = y;
  }
} // namespace

void Fl_RGB_Image::draw(int XP, int YP, int WP, int HP, int cx, int cy) {
  if ((cx != 0 || cy != 0 || WP != w() || HP != h()) && w() == data_w() && h() == data_h()) {
    RectangleInt r1 = { XP - cx, YP - cy, w(), h() };
    const RectangleInt r2 = { XP, YP, WP, HP };
    crect_intersect(&r1, &r2);
    if (r1.width == 0 || r1.height == 0) {
      return;
    }

    const int l = (ld() != 0 ? ld() : d() * w());
    const uchar *p = array + static_cast<size_t>(fl_max(cy, 0)) * l + fl_max(cx, 0) * d();

    auto *temp_rgb = new Fl_RGB_Image(p, r1.width, r1.height, d(), l);
    fl_graphics_driver->draw_rgb(temp_rgb, r1.x, r1.y, r1.width, r1.height, 0, 0);
    delete temp_rgb;
  } else {
    fl_graphics_driver->draw_rgb(this, XP, YP, WP, HP, cx, cy);
  }
}

void Fl_RGB_Image::label(Fl_Widget* widget) {
  widget->image(this);
}

void Fl_RGB_Image::label(Fl_Menu_Item* m) {
  m->label(FL_IMAGE_LABEL, reinterpret_cast<const char*>(this));
}