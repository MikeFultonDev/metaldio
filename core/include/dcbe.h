#ifndef __DCBE__
#define __DCBE__ 1

#include "metaldio.h"

#pragma pack(1)

struct dcbe {
  unsigned char  dcbe_hdr[4];   /* 0  Alignment and identifier (DCBE)         */
  short int      dcbe_len;      /* 4  DCBE V0 length, min is 56'        @L8A  */
  unsigned char  _filler2[2];   /* 6  Reserved, should be zero                */
  void* PTR32  _filler3;      /* 8  0 if not open, OPEN points to DCB       */
  unsigned char  _filler4[4];   /* C  Disk address of current member          */
  unsigned char  _filler5;      /* 10  Flags set by system                    */
  unsigned char  usrflags;      /* 11  Flags set by user                      */
  short int      _filler7;      /* 12  Number of stripes if extended format   */
  unsigned char  _filler8;      /* 14  Flags set by user                 @L3A */
  unsigned char  _filler9;      /* 15  Flags                             @L9A */
  unsigned char  _filler10[2];  /* 16  Reserved                          @L9C */
  unsigned char  _filler11[4];  /* 18  Reserved                          @L2A */
  int            _filler12;     /* 1C  Block size                        @L2A */
  unsigned char  _filler13[8];  /* 20  Reserved & number of blocks in ds @L2C */
  void * PTR32 eodad;         /* 28  End of data routine address or 0       */
  void * PTR32 synad;         /* 2C  I/O error routine (synchronous) or 0   */
  unsigned char  _filler16[4];  /* 30  Reserved, should be zero          @MAC */
  short int      _filler17;     /* 34  tape files written before sync    @MAA */
  unsigned char  _filler18[2];  /* 36  MULTACC and MULTSDN                    */
  };

#pragma pack(pop)

#endif
