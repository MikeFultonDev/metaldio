#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include "asmdio.h"
#include "dio.h"
#include "mem.h"
#include "stow.h"
#include "s99.h"
#include "wrappers.h"

//#define DEBUG 1

const struct s99_rbx s99rbxtemplate = {"S99RBX",S99RBXVR,{0,1,0,0,0,0,0},0,0,0};

static unsigned int complement(unsigned int x) {
  return (~x) + 1;
}

int STOW(union stowlist* PTR32 listp, struct ihadcb* PTR32 dcbp, enum stowtype type)
{
  /*
   * Need to set the bits on the dcb and list pointers, so make
   * them 32 bit to ensure C does not try to do anything 'fancy'
   *
   * For the default case, the dcb should be NULL since it
   * should have been stored into the list already
   */
  unsigned int list = (unsigned int) listp;
  unsigned int dcb = (unsigned int) dcbp;

  /*
   * Clear the high order bit to get to a 'well known state'
   */
  list &= 0x7FFFFFFF;
  dcb  &= 0x7FFFFFFF;

  switch (type) {
    case STOW_A:
      /* list and dcb should be positive - nothing required */
      break;
    case STOW_R:
      /* list positive, dcb complement */
      dcb = complement(dcb);
      break;
    case STOW_D:
      /* list complement, dcb positive */
      list = complement(list);
      break;
    case STOW_C:
      /* list complement, dcb complement */
      list = complement(list);
      dcb = complement(list);
      break;
    case STOW_I:
      /* list should be NULL, dcb positive */
      if (list) {
      #ifdef DEBUG
        fprintf(stderr, "stowlist pointer should be NULL for the INIT stowtype.\n");
      #endif
        return -1;
      }
      break;
    default:
      if (dcb) {
      #ifdef DEBUG
        fprintf(stderr, "DCB pointer should be NULL for the default stowtype - DCB is in stowlist\n");
      #endif
        return -1;
      }
      break;
  }
  return STOWA(list, dcb);
}

int S99(struct s99rb* PTR32 s99rb)
{
  return S99A(s99rb);
}
int S99MSG(struct s99_em* PTR32 s99em)
{
  return S99MSGA(s99em);
}
int SYEXENQ(char* PTR32 qname, char* PTR32 rname, unsigned int rname_len)
{
  return SYEXENQA(qname, rname, rname_len);
}
int SYEXDEQ(char* PTR32 qname, char* PTR32 rname, unsigned int rname_len)
{
  return SYEXDEQA(qname, rname, rname_len);
}

jmp_buf abend_jmp_buf;

void abend_handler(int val)
{
  longjmp(abend_jmp_buf, 1);
}

int OPEN(struct opencb* PTR32 opencb)
{
  if (signal(SIGABND, abend_handler) == SIG_ERR) {
    perror("Internal error - unable to set up abend handler");
    return -2;
  }
  if (setjmp(abend_jmp_buf)) {
    /*
     * Signal handler returned control to me - mark OPEN as 'fail'
     */
    return -1;
  }
  return OPENA(opencb);
}

int FIND(struct findcb* PTR32 findcb, struct ihadcb* PTR32 dcb)
{
  return FINDA(findcb, dcb);
}
int READ(struct decb* PTR32 decb)
{
  return READA(decb);
}
int WRITE(struct decb* PTR32 decb)
{
  return WRITEA(decb);
}
int CHECK(struct decb* PTR32 decb)
{
  return CHECKA(decb);
}
unsigned NOTE(struct ihadcb* PTR32 dcb)
{
  return NOTEA(dcb);
}
unsigned POINT(struct ihadcb* PTR32 dcb, unsigned int ttr)
{
  return POINTA(dcb, ttr);
}
unsigned DESERV(struct desp* PTR32 desp)
{
  return DESERVA(desp);
}
int CLOSE(struct closecb* PTR32 closecb)
{
  return CLOSEA(closecb); 
}
