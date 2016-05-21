bin_PROGRAMS = viterbi
AM_CXXFLAGS = $(libpng_CFLAGS) $(zlib_CFLAGS) $(eigen3_CFLAGS) $(CFLAGS) -std=c++0x
viterbi_SOURCES = viterbi.cc viterbi2d.cc cache.cc
viterbi_LDADD = $(libpng_LIBS) $(zlib_LIBS) -ljson-c -lm $(LDFLAGS)
