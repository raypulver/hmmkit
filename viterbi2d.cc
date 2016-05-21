#include "viterbi2d.h"
#include <cmath>
#include <cstdlib>
#include <functional>
#include <iostream>
#include "cache.h"
#include "hmm2d.h"
#include "sum.h"

using namespace std;

void RandomWalk(HMM2D *hmm, bool isy, size_t start, size_t len, long double thresh,
                function<void(size_t, long double prob, size_t sublen)> fn,
                vector<HMM2D::State> &seq, long double prob) {
  double newprob;
  fn(Sum(seq), prob, len - seq.size());
  if (len != seq.size()) {
    for (size_t i = 0; i < hmm->states.size(); ++i) {
      if (isy)
        newprob = prob * hmm->ax(hmm->state_map[seq.back()], i);
      else
        newprob = prob * hmm->ay(hmm->state_map[seq.back()], i);
      if (newprob > thresh) {
        seq.push_back(hmm->states[i]);
        RandomWalk(hmm, isy, start, len, thresh, fn, seq, newprob);
        seq.pop_back();
      }
    }
  }
}

void RandomWalk(HMM2D *hmm, bool isy, size_t start, size_t len, long double thresh,
                function<void(size_t, long double prob, size_t sublen)> fn) {
  vector<HMM2D::State> seq;
  long double prob = 1;
  seq.push_back(hmm->states[start]);
  RandomWalk(hmm, isy, start, len, thresh, fn, seq, prob);
}
long double Prob(HMM2D *hmm, Cache<Viterbi2DResult *> &cache, Cache<long double *> &probcache, size_t s,
                 size_t t, size_t k, bool isy) {
  long total = 0;
  size_t st;
  double prb = 0, max = 0;
  long double *retval;
  Viterbi2DResult *result;
  if ((retval = *probcache.Get(1, hmm->observations.size() - t + 1, k))) {
    return *retval;
  }
  for (size_t i = 0; i < t; ++i) {
    st = 0;
    for (size_t n = 0; n < hmm->states.size(); ++n) {
      if ((result = *cache.Get(s, i, n))) {
        if (result->probability > max) {
          st = n;
          max = result->probability;
        }
      }
    }
    total += hmm->states[st];
    max = 0;
  }
  total = hmm->observations[s] - total;
  if (total < 0) return 0;
  long double mean = 0, var = 0, totalprob = 0;
  size_t count = 0;
  RandomWalk(hmm, isy, k, hmm->observations.size() - t + 1, 1e-10,
             [&](size_t sum, long double prob, size_t len) {
               probcache.CopyPut(1, len, k, &prob);
               if (len == hmm->observations.size() - t + 1 && sum == total) {
                 cout << sum << " " << prob << " " << total << endl;
                 prb += prob;
               }
             });
  return prb;
}
Viterbi2DResult::~Viterbi2DResult() {
  if (lastx) delete lastx;
  if (lasty) delete lasty;
}

Viterbi2DResult *Viterbi2DMax(HMM2D *hmm, size_t s, size_t t,
                              Cache<Viterbi2DResult *> &cache, Cache<long double *> &probcache) {
  size_t i;
  double max;
  Viterbi2DResult *result, *retval;
  max = 0;
  result = nullptr;
  retval = nullptr;
  for (size_t i = 0; i < hmm->states.size(); ++i) {
    result = Viterbi2D(hmm, cache, probcache, s, t, i);
    if (result->probability > max) {
      retval = result;
      max = result->probability;
    }
  }
  return retval;
}

Viterbi2DResult *Viterbi2D(HMM2D *hmm, Cache<Viterbi2DResult *> &cache, Cache<long double *> &probcache,
                           size_t s, size_t t, size_t k) {
  Viterbi2DResult *retval, *xviterbi, *yviterbi;
  long double max, overall, tmp;
  size_t x, y;
  max = 0;
  overall = 0;
  if ((retval = *cache.Get(s, t, k))) {
    return retval;
  }
  retval = new Viterbi2DResult();
  retval->probability = 1;
  tmp = retval->probability;
  if (!s && !t) {
    retval->probability *= sqrt(hmm->pix[k] * hmm->piy[k]);
  } else if (!s) {
    for (y = 0; y < hmm->states.size(); ++y) {
      yviterbi = Viterbi2D(hmm, cache, probcache, 0, t - 1, y);
      overall = tmp * sqrt(hmm->pix[k] * hmm->ay(y, k) * yviterbi->probability);
      if (overall > max) {
        max = overall;
        retval->y = y;
        retval->lasty = yviterbi;
        retval->probability = overall;
      }
    }
  } else if (!t) {
    for (x = 0; x < hmm->states.size(); ++x) {
      xviterbi = Viterbi2D(hmm, cache, probcache, s - 1, 0, x);
      overall = tmp * sqrt(hmm->piy[k] * hmm->ax(x, k) * xviterbi->probability);
      if (overall > max) {
        max = overall;
        retval->x = x;
        retval->lastx = xviterbi;
        retval->probability = overall;
      }
    }
    retval->probability *= hmm->piy[k];
  } else {
    for (x = 0; x < hmm->states.size(); ++x) {
      for (y = 0; y < hmm->states.size(); ++y) {
        xviterbi = Viterbi2D(hmm, cache, probcache, s - 1, t, x);
        yviterbi = Viterbi2D(hmm, cache, probcache, s, t - 1, y);
        overall = tmp * sqrt(hmm->ax(x, k) * xviterbi->probability *
                             hmm->ay(y, k) * yviterbi->probability);
        if (overall > max) {
          max = overall;
          retval->y = y;
          retval->x = x;
          retval->lastx = xviterbi;
          retval->lasty = yviterbi;
        }
      }
    }
  }
  retval->probability *= Prob(hmm, cache, probcache, s, t, k, false);
  cache.Put(s, t, k, retval);
  return retval;
}
int Reconstruct(HMM2D *hmm, Cache<Viterbi2DResult *> &cache,
                const char *filename) {
  double max;
  size_t depth;
  PNG<PNG_FORMAT_GA> png(cache.GetWidth(), cache.GetHeight());
  PNG<PNG_FORMAT_GA>::Pixel *pxls = png.GetPixelArray();
  for (size_t i = 0; i < cache.GetWidth(); ++i) {
    for (size_t j = 0; j < cache.GetHeight(); ++j) {
      max = 0;
      depth = 0;
      for (size_t k = 0; k < cache.GetDepth(); ++k) {
        if (*cache.Get(i, j, k) && (*cache.Get(i, j, k))->probability > max) {
          max = (*cache.Get(i, j, k))->probability;
          depth = hmm->states[k];
        }
      }
      pxls[i * cache.GetHeight() + j].g = depth;
      if (pxls[i * cache.GetHeight() + j].g)
        pxls[i * cache.GetHeight() + j].a = 0xff;
    }
  }
  return png.Write(filename);
}
