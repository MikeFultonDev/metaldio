#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "wrappers.h"

//#define DEBUG 1

void* PTR32 MALLOC24(unsigned int bytes)
{
  int ptr24;
  ptr24 = MALOC24A(bytes);
  void* PTR32 ptr = (void* PTR32) ptr24;
  if (ptr24 == 0) {
    fprintf(stderr, "Internal Error: Unable to allocate %d bytes below the bar\n", bytes);
  } else {
#ifdef DEBUG
  memset(ptr, 0xFE, bytes);
#else
  memset(ptr, 0x00, bytes);
#endif
  }
  return ptr;
}

int FREE24(void* PTR32 addr, unsigned int len)
{
  return FREE24A(addr, len);
}

void* PTR32 MALLOC31(unsigned int bytes)
{
  void* PTR32 p = __malloc31(bytes);
  if (p == 0) {
    fprintf(stderr, "Internal Error: Unable to allocate %d bytes below the line\n", bytes);
  } else {
#ifdef DEBUG
  memset(p, 0xFE, bytes);
#else
  memset(p, 0x00, bytes);
#endif
  }
  return p;
}
void FREE31(void* PTR32 addr)
{
  return free(addr);
}

void dumpstg(const DBG_Opts* opts, void* p, size_t len)
{
  char* buff = p;
  size_t i;
  for (i=0; i<len; ++i) {
    if ((i != 0) && (i % 16 == 0)) {
      errmsg(opts, "\n");
    }
    if (i % 4 == 0) {
      errmsg(opts, " ");
    }
    errmsg(opts, "%2.2X", buff[i]);
  }
}
