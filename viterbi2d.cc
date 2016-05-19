#include "viterbi2d.h"
#include <iostream>
#include <cmath>
#include <functional>
#include "hmm2d.h"
#include "cache.h"

/*
void RandomWalk(AMatrix &mat, size_t start, size_t len, double thresh,
function<void(size_t, double prob)>) {

long double Prob(HMM2D *hmm, size_t s, size_t t, size_t k, bool isy) {
*/
Viterbi2DResult::~Viterbi2DResult() {
  if (lastx) delete lastx;
  if (lasty) delete lasty;
}

Viterbi2DResult *Viterbi2DMax(HMM2D *hmm, size_t s, size_t t) {
  size_t i;
  double max;
  Viterbi2DResult *result, *retval;
  max = 0;
  result = nullptr;
  retval = nullptr;
  Cache<Viterbi2DResult *> cache (s, t, hmm->states.size());
  for (size_t i = 0; i < hmm->states.size(); ++i) {
    result = Viterbi2D(hmm, cache, s, t, i);
    if (result->probability > max) {
      if (retval) delete retval;
      retval = result;
      max = result->probability;
    } else
      delete result;
  }
  return retval;
}

Viterbi2DResult *Viterbi2D(HMM2D *hmm, Cache<Viterbi2DResult *> &cache, size_t s, size_t t, size_t k) {
  Viterbi2DResult *retval, *xviterbi, *yviterbi;
  long double max, overall;
  size_t x, y;
  // retval->probability = sqrt(Prob(hmm, s, t, k, false) * Prob(hmm, s, t, k,
  // true));
  max = 0;
  overall = 0;
  if ((retval = *cache.Get(s, t, k))) {
    return retval;
  }
  retval = new Viterbi2DResult();
  retval->probability = 1;
  if (!s && !t) {
    retval->probability *= sqrt(hmm->pix[k] * hmm->piy[k]);
    return retval;
  }
  if (!s) {
    retval->probability *= hmm->pix[k];
    return retval;
  }
  if (!t) {
    retval->probability *= hmm->piy[k];
    return retval;
  }
  for (x = 0; x < hmm->states.size(); ++x) {
    for (y = 0; y < hmm->states.size(); ++y) {
      xviterbi = Viterbi2D(hmm, cache, s - 1, t, x);
      yviterbi = Viterbi2D(hmm, cache, s, t - 1, y);
      overall = retval->probability *
                sqrt(xviterbi->probability * yviterbi->probability);
      if (overall > max) {
        max = overall;
        if (retval->lastx) delete retval->lastx;
        if (retval->lasty) delete retval->lasty;
        retval->y = y;
        retval->x = x;
        retval->lastx = xviterbi;
        retval->lasty = yviterbi;
      } else {
        delete xviterbi;
        delete yviterbi;
      }
    }
  }
  return retval;
}
