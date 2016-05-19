#ifndef DIECLASS_H
#define DIECLASS_H
#include <cstring>

class DieClass {
  static constexpr size_t MAX_ERROR_LEN = 1 << 16;
  const char *basename;
  void Death() {
    exit(1);
  }
  public:
    static DieClass Init(const char *basename) {
      DieClass retval;
      retval.basename = basename;
      return retval;
    }
    template <typename T = const char *, typename... Args>
    void operator()(T fmt, Args ...args) {
      char buf[MAX_ERROR_LEN];
      memset(buf, 0, sizeof(buf));
      sprintf(buf, fmt, args...);
      printf("%s: %s", basename, buf);
      Death();
    }
    void operator()(const char *fmt) {
      printf("%s: %s", basename, fmt);
      Death();
    }
};

#endif
