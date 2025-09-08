#ifndef __MEM_H__
#define __MEM_H__ 1

  #include <stdio.h>
  #include "metaldio.h"
  #include "dbgopts.h"

  void* PTR32 MALLOC24(unsigned int len);
  int FREE24(void* PTR32 addr, unsigned int len);
  void* PTR32 MALLOC31(unsigned int len);
  void FREE31(void* PTR32 addr);

  void dumpstg(const DBG_Opts* opts, void* p, size_t len);


#endif
