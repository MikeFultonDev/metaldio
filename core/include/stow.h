#ifndef __STOW__
#define __STOW__ 1

#include "metaldio.h"

#pragma pack(1)

struct stowlist_add {
  char mem_name[8];
  unsigned short tt;
  unsigned char r;
  unsigned char c;
  unsigned char user_data[62];
};

#define STOW_SET_TTR(add, ttr) \
  ( \
  ( (add).tt = ((ttr) & 0xFFFF0000) >> 16), \
  ( (add).r = ((ttr) & 0xFF00) >> 8) \
  )

struct stowlist_generic_includes_dcb {
  unsigned short list_len;
  unsigned char type;
  unsigned short reserved;
  int dcb24: 24;
  /* more stuff */
};

#define STOWLIST_IFF_TIMESTAMP_LEN (8)
struct stowlist_iff {
  unsigned short list_len;
  unsigned char type;
  unsigned char reserved;
  int dcbHOB: 8;
  int dcb24: 24;
  char timestamp[STOWLIST_IFF_TIMESTAMP_LEN];
  void* PTR32 direntry;
  char user_descriptor[16];
  unsigned short ccsid;
};

union stowlist {
  struct stowlist_generic_includes_dcb;
  struct stowlist_iff iff;
  struct stowlist_add add;
};

enum stowtype {
  STOW_A=1,
  STOW_C=2,
  STOW_D=3,
  STOW_I=4,
  STOW_R=5,
  STOW_DISC=0x80,
  STOW_IFF=0x40,
  STOW_RG=0x20,
  STOW_DG=0x10,
  STOW_RECOVERG=0x08
};

enum stowcc {
  STOW_CC_OK=0,
  STOW_IFF_CC_CREATE_OK=4,
  STOW_IFF_CC_MEMBER_EXISTS=8,
  STOW_IFF_CC_PDS_UPDATE_UNSUPPORTED=0x40028
};
#define STOW_REPLACE_MEMBER_DOES_NOT_EXIST (8)

#pragma pack(pop)

#endif
