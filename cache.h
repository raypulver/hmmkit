#ifndef CACHE_H
#define CACHE_H

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <type_traits>

template <typename T>
class TripleArray {
  typedef TripleArray<T> self;
  T *data;
  size_t x, y, z;

 public:
  inline size_t GetWidth() { return x; }
  inline size_t GetHeight() { return y; }
  inline size_t GetDepth() { return z; }
  void Resize(size_t x, size_t y, size_t z) {
    if (!data) {
      data = (T *)malloc(x * y * z * sizeof(T));
    } else
      data = (T *)realloc(data, x * y * z * sizeof(T));
    this->x = x;
    this->y = y;
    this->z = z;
  }
  void Zero() { memset(data, 0, x * y * z); }
  TripleArray() { memset(this, 0, sizeof(self)); }
  TripleArray(size_t x, size_t y, size_t z) {
    this->x = x;
    this->y = y;
    this->z = z;
    data = (T *)malloc(x * y * z * sizeof(T));
    memset(data, 0, x * y * z * sizeof(T));
  }
  ~TripleArray() { free(data); }
  T *operator()(size_t x, size_t y, size_t z) {
    return &data[x * this->y * this->z + y * this->z + z];
  }
};

struct ProbCache {
 private:
  long double *data;
  size_t len;
  size_t states;
 public:
  ProbCache(size_t len, size_t states);
  ~ProbCache();
  long double &operator()(size_t len, size_t state);
  void Put(size_t len, size_t state, long double d);
  void Add(size_t len, size_t state, long double d);
  long double &Get(size_t len, size_t state);
};


template <typename T>
struct Cache {
  TripleArray<T> data;
  Cache(size_t x, size_t y, size_t z) {
    data.Resize(x, y, z);
    data.Zero();
  }
  T *Get(size_t x, size_t y, size_t z) { return data(x, y, z); }
  void Put(size_t x, size_t y, size_t z, T el) { *data(x, y, z) = el; }
  size_t GetWidth() { return data.GetWidth(); }
  size_t GetHeight() { return data.GetHeight(); }
  size_t GetDepth() { return data.GetDepth(); }
  void Print() {
    for (size_t i = 0; i < GetWidth(); ++i) {
      for (size_t j = 0; j < GetHeight(); ++j) {
        for (size_t k = 0; k < GetDepth(); ++k) {
          if (*Get(i, j, k)) {
            std::cout << i << " " << j << " " << k << " "
                      << (*Get(i, j, k))->probability << std::endl;
          }
        }
      }
    }
  }
};

#endif
