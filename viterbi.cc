#include <getopt.h>
#include <libgen.h>
#include <cstring>
#include <cmath>
#include <ctime>
#include <iostream>
#include "pixelizer.h"
#include "hmm2d.h"
#include "viterbi2d.h"
#include "die.h"

using namespace std;

static struct option long_options[] = {
  {"reconstruct", no_argument, 0, 'r'},
  {0, 0, 0, 0}
};

static enum class Mode {
  GENERATE,
  SOLVE
} mode;

static DieClass Die;

void generate(int dim) {
  if (dim <= 0) Die("need a positive dimension");
  PNG<PNG_FORMAT_GA> png(dim, dim);
  PNG<PNG_FORMAT_GA>::Pixel *buf = png.GetPixelArray();
  for (size_t i = 0; i < dim; ++i) {
    for (size_t j = 0; j < dim; ++j) {
      if (j < i/(sin(M_PI/4)/cos(M_PI/4))) {
        buf[i*dim + j].g = 170;
        buf[i*dim + j].a = 0xff;
      } else if (j < (i*1.5)/(sin(M_PI/4)/cos(M_PI/4))) {
        buf[i*dim + j].g = 120;
        buf[i*dim + j].a = 0xff;
      }
    }
  }
  png.Write("out.png");
}

void solve(bool reconstruct) {
  PNG<PNG_FORMAT_GA> *png = PNG<PNG_FORMAT_GA>::FromFile("out.png");
  HMM2D *hmm = HMM2D::FromPNG(png);
  double start = clock();
  cout << png->GetWidth() << " " << png->GetHeight() << endl;
  Viterbi2DMax(hmm, png->GetWidth(), png->GetHeight());
  cout << clock() - start << endl;
}

int main(int argc, char **argv) {
  int c, long_index, dim = 16;
  bool reconstructit = false, parsingdim = false;
  Die = DieClass::Init(basename(argv[0]));
  while ((c = getopt_long(argc, argv, "r", long_options, &long_index)) != -1) {
    switch (c) {
      case 'r':
        reconstructit = true;
        break;
    }
  }
  while (optind < argc) {
    if (parsingdim) {
      dim = atoi(argv[optind]);
      parsingdim = false;
    } else if (!strcmp(argv[optind], "generate")) {
      mode = Mode::GENERATE;
      parsingdim = true;
    } else if (!strcmp(argv[optind], "solve")) {
      mode = Mode::SOLVE;
    }
    ++optind;
  }
  if (mode == Mode::GENERATE) {
    generate(dim);
  } else if (mode == Mode::SOLVE) {
    solve(reconstructit);
  }
  return 0;
}
