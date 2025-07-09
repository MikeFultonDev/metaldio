#ifndef __OPENCB__
#define __OPENCB__ 1

#include "metaldio.h"

#pragma pack(1)
struct opencb {
  int last_entry:1;
  int disp:3;
  int mode:4;
  int reserved:24;
  void* PTR32 dcb24;
};

#define OPEN_INPUT (0)
#define OPEN_OUTPUT (0xF)
#pragma pack(pop)

#endif
