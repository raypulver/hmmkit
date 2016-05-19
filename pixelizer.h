#ifndef PIXELIZER_H
#define PIXELIZER_H

#include <png.h>

template <int format>
struct PNGPixel;
template <>
struct PNGPixel<PNG_FORMAT_GA> {
  uint8_t g;
  uint8_t a;
  size_t GetValue() { return g; }
};
template <>
struct PNGPixel<PNG_FORMAT_GRAY> {
  uint8_t g;
  size_t GetValue() { return g; }
};
template <>
struct PNGPixel<PNG_FORMAT_AG> {
  uint8_t a;
  uint8_t g;
  size_t GetValue() { return g; }
};
template <>
struct PNGPixel<PNG_FORMAT_RGB> {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  size_t GetValue() { return (size_t)r << 16 | (size_t)g << 8 | (size_t)b; }
};
template <>
struct PNGPixel<PNG_FORMAT_BGR> {
  uint8_t b;
  uint8_t g;
  uint8_t r;
  size_t GetValue() { return (size_t)r << 16 | (size_t)g << 8 | (size_t)b; }
};
template <>
struct PNGPixel<PNG_FORMAT_RGBA> {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
  size_t GetValue() { return (size_t)r << 16 | (size_t)g << 8 | (size_t)b; }
};
template <>
struct PNGPixel<PNG_FORMAT_ARGB> {
  uint8_t a;
  uint8_t r;
  uint8_t g;
  uint8_t b;
  size_t GetValue() { return (size_t)r << 16 | (size_t)g << 8 | (size_t)b; }
};
template <>
struct PNGPixel<PNG_FORMAT_BGRA> {
  uint8_t b;
  uint8_t g;
  uint8_t r;
  uint8_t a;
  size_t GetValue() { return (size_t)r << 16 | (size_t)g << 8 | (size_t)b; }
};
template <>
struct PNGPixel<PNG_FORMAT_ABGR> {
  uint8_t a;
  uint8_t b;
  uint8_t g;
  uint8_t r;
  size_t GetValue() { return (size_t)r << 16 | (size_t)g << 8 | (size_t)b; }
};
template <int format>
class PNG {
  void *buffer;
  void *colormap;
  png_image img;

 public:
  typedef PNGPixel<format> Pixel;
  PNG(int x, int y) {
    img.width = x;
    img.height = y;
    img.version = PNG_IMAGE_VERSION;
    img.opaque = nullptr;
    img.format = format;
    img.flags = 0;
    img.colormap_entries = 0;
    buffer = calloc(x * y, sizeof(PNG<format>::Pixel));
    memset(buffer, 0, x * y * sizeof(PNG<format>::Pixel));
    colormap = nullptr;
  }
  ~PNG() {
    if (buffer) free(buffer);
    if (colormap) free(colormap);
  }
  int GetWidth() { return img.width; }
  int GetHeight() { return img.height; }
  int Write(std::string filename) { return Write(filename.c_str()); }
  int Write(const char *filename) {
    return png_image_write_to_file(&img, filename, 0, buffer, 0, colormap);
  }
  void *GetBuffer() { return buffer; };
  void SetBuffer(void *buf) {
    if (buffer) free(buffer);
    buffer = buf;
  }
  PNG<format>::Pixel *GetPixelArray() { return (PNG<format>::Pixel *)buffer; }
  static PNG<format> *FromFile(std::string filename) {
    png_image img;
    memset(&img, 0, (sizeof(png_image)));
    img.version = PNG_IMAGE_VERSION;
    if (png_image_begin_read_from_file(&img, filename.c_str())) {
      png_bytep buffer;
      img.format = format;
      buffer = (png_bytep)malloc(PNG_IMAGE_SIZE(img));
      if (buffer != nullptr &&
          png_image_finish_read(&img, NULL, buffer, 0, NULL)) {
        PNG<format> *retval = new PNG<format>(img.width, img.height);
        retval->SetBuffer(buffer);
        return retval;
      }
    }
    return nullptr;
  }
};
#endif
