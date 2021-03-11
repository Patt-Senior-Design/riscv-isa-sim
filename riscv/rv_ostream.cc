#include "rv_ostream.h"
#include "common.h"
#include <cstdlib>
#include <cstdio>
#include <cstdarg>

void rv_ostream_t::printf(const char *fmt, ...)
{
  va_list ap;
  char buf[128];
  char *heapbuf = nullptr;
  char *str = buf;

  va_start(ap, fmt);
  unsigned len = vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  if (unlikely(len >= sizeof(buf))) {
    heapbuf = (char *) malloc(len+1);

    va_start(ap, fmt);
    vsnprintf(heapbuf, len+1, fmt, ap);
    va_end(ap);

    str = heapbuf;
  }

  ostream << str;
  if (unlikely(heapbuf != nullptr)) {free(heapbuf);}
}
