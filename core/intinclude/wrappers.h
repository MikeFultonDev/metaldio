/*
 * wrappers for C 31-bit or 64-bit functions
 */

#include "metaldio.h"

#if AMODE==31
  #if 0
  #pragma linkage(STOWA,    OS)
  #pragma linkage(NOTEA,    OS)
  #pragma linkage(POINTA,   OS)
  #pragma linkage(DESERVA,  OS)
  #pragma linkage(S99A,     OS)
  #pragma linkage(S99MSGA,  OS)
  #pragma linkage(OPENA,    OS)
  #pragma linkage(CLOSEA,   OS)
  #pragma linkage(MALOC24A, OS)
  #pragma linkage(FREE24A,  OS)
  #endif

  struct s99rb;
  struct opencb;
  struct findcb;
  struct s99_em;
  struct ihadcb;
  struct decb;
  struct closecb;
  struct desp;

  int S99A(struct s99rb* PTR32 rb);
  int S99MSGA(struct s99_em* PTR32 em);
  int OPENA(struct opencb* PTR32 opencb);
  int FINDA(struct findcb* PTR32 findcb, struct ihadcb* PTR32 dcb);
  int READA(struct decb* PTR32 decb);
  int WRITEA(struct decb* PTR32 decb);
  int CHECKA(struct decb* PTR32 decb);
  unsigned int NOTEA(struct ihadcb* PTR32 dcb);
  unsigned int POINTA(struct ihadcb* PTR32 dcb, unsigned int ttr);
  int DESERVA(struct desp* PTR32 desp);
  int STOWA(unsigned int list, unsigned int dcb);
  int CLOSEA(struct closecb* PTR32 dcb);

  int MALOC24A(unsigned int len);

  int FREE24A(void* __ptr32 addr, unsigned int len);
  int SYEXENQA(char* __ptr32 qname, char* __ptr32 rname, unsigned int rname_len);
  int SYEXDEQA(char* __ptr32 qname, char* __ptr32 rname, unsigned int rname_len);

#elif AMODE == 64
  int call31asm(const char* fn_name, int* fn, unsigned int num_parms, ...);

  extern int S99A;
  extern int S99MSGA;
  extern int SYEXENQA;
  extern int SYEXDEQA;
  extern int OPENA;
  extern int FINDA;
  extern int READA;
  extern int WRITEA;
  extern int CHECKA;
  extern int NOTEA;
  extern int POINTA;
  extern int DESERVA;
  extern int STOWA;
  extern int CLOSEA;
  extern int MALOC24A;
  extern int FREE24A;
  #pragma variable(S99A,     NORENT)
  #pragma variable(S99MSGA,  NORENT)
  #pragma variable(SYEXENQA,  NORENT)
  #pragma variable(SYEXDEQA,  NORENT)
  #pragma variable(OPENA,    NORENT)
  #pragma variable(FINDA,    NORENT)
  #pragma variable(READA,    NORENT)
  #pragma variable(WRITEA,   NORENT)
  #pragma variable(CHECKA,   NORENT)
  #pragma variable(NOTEA,    NORENT)
  #pragma variable(POINTA,   NORENT)
  #pragma variable(DESERVA,  NORENT)
  #pragma variable(STOWA,    NORENT)
  #pragma variable(CLOSEA,   NORENT)
  #pragma variable(MALOC24A, NORENT)
  #pragma variable(FREE24A,  NORENT)
  
	#define S99A(ptr)         call31asm("S99A", &S99A, 1, ptr)
	#define S99MSGA(ptr)      call31asm("S99MSGA", &S99MSGA, 1, ptr)
	#define SYEXENQA(qname,rname,rnamelen)      call31asm("SYEXENQA", &SYEXENQA, 3, qname, rname, rnamelen)
	#define SYEXDEQA(qname,rname,rnamelen)      call31asm("SYEXDEQA", &SYEXDEQA, 3, qname, rname, rnamelen)
	#define OPENA(opencb)     call31asm("OPENA", &OPENA, 1, opencb)
	#define FINDA(findcb,dcb) call31asm("FINDA", &FINDA, 2, findcb, dcb)
	#define READA(decb)       call31asm("READA", &READA, 1, decb)
	#define WRITEA(decb)      call31asm("WRITEA", &WRITEA, 1, decb)
	#define CHECKA(decb)      call31asm("CHECKA", &CHECKA, 1, decb)
	#define NOTEA(decb)       call31asm("NOTEA", &NOTEA, 1, decb)
	#define POINTA(decb,ttr)  call31asm("POINTA", &POINTA, 2, decb, ttr)
	#define DESERVA(desp)     call31asm("DESERVA", &DESERVA, 1, desp)
	#define STOWA(lst,dcb)    call31asm("STOWA", &STOWA, 2, list, dcb)
	#define CLOSEA(closecb)   call31asm("CLOSEA", &CLOSEA, 1, closecb)
	#define MALOC24A(len)     call31asm("MALOC24A", &MALOC24A, 1, len)
	#define FREE24A(ptr,len)  call31asm("FREE24A", &FREE24A, 2, ptr, len)
#else
	#error "AMODE must be 31 or 64"
#endif
