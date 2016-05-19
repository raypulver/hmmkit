#ifndef VITERBI2D_H
#define VITERBI2D_H
#include <unistd.h>
#include "hmm2d.h"
#include "cache.h"

struct Viterbi2DResult {
  typedef Viterbi2DResult self;
 public:
  ~Viterbi2DResult();
  long double probability;
  self *lastx;
  self *lasty;
  size_t x;
  size_t y;
};

Viterbi2DResult *Viterbi2D(HMM2D *hmm, Cache<Viterbi2DResult *> &cache, size_t s, size_t t, size_t k);
Viterbi2DResult *Viterbi2DMax(HMM2D *hmm, size_t s, size_t t);
#endif
