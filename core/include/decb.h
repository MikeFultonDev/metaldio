#ifndef __DECB__
#define __DECB__ 1

#include "metaldio.h"

#pragma pack(1)
struct decb {
  unsigned int ecb;
  unsigned short type;
  unsigned short length;
  unsigned int dcb24_hoB: 8;
  unsigned int dcb24: 24;
  void* PTR32 area;
  void* PTR32 stat_addr;
  void* PTR32 key_addr;
  void* PTR32 block_addr;
  void* PTR32 next_addr;
};
#pragma pack(pop)

#endif
