#ifndef CACHE_H
#define CACHE_H

#include <cstring>
#include <cstdlib>

template <typename T> class TripleArray {
  typedef TripleArray<T> self;
  T *data;
  size_t x, y, z;
 public:
  inline size_t GetWidth() { return x; }
  inline size_t GetHeight() { return y; }
  inline size_t GetDepth() { return z; }
  void Resize(size_t x, size_t y, size_t z) {
    if (!data) {
      data = (T *) malloc(x*y*z*sizeof(T));
    } else data = (T *) realloc(data, x*y*z*sizeof(T));
  }
  void Zero() {
    memset(data, 0, x*y*z);
  }
  TripleArray() {
    memset(this, 0, sizeof(self));
  }
  TripleArray(size_t x, size_t y, size_t z) {
    this->x = x;
    this->y = y;
    this->z = z;
    data = (T *) malloc(x*y*z*sizeof(T));
    memset(data, 0, x*y*z*sizeof(T));
  }
  ~TripleArray() {
    free(data);
  }
  T *operator()(size_t x, size_t y, size_t z) {
    return &data[x*this->y*this->z + y*this->z + y];
  }
};

template <typename T>
struct Cache {
  TripleArray<T> data;
  Cache(size_t x, size_t y, size_t z) {
    data.Resize(x, y, z);
    data.Zero();
  }
  T *Get(size_t x, size_t y, size_t z) {
    return data(x, y, z);
  }
  void Put(size_t x, size_t y, size_t z, T el) {
    *data(x, y, z) = el;
  }
};
#endif
