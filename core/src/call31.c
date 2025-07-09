#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "metaldio.h"
#include "wrappers.h"

#if AMODE == 64
	// msf - eventually move the DSA allocation and parms allocation to a per-thread allocation and re-use
	#define MAX_DSA_SIZE 4096

	#pragma linkage(CALL31A, OS)
	int CALL31A(int* fn, char* dsa, unsigned int* parms);

//#define DEBUG_CALL31ASM 1

	int call31asm(const char* fn_name, int* fn, unsigned int num_parms, ...) {
		   va_list args;
		   unsigned int i;
		   int rc;
		   unsigned int* r1_31bit_parms = __malloc31(num_parms*sizeof(unsigned int));
		   char* r13_31bit_dsa = __malloc31(MAX_DSA_SIZE);

		   va_start(args, num_parms);
#if DEBUG_CALL31ASM
		   tracemsg("Function Name: %s Function Pointer:0x%p", LOGMSG_CLASS_VERBOSE, fn_name, fn);
		   tracemsg("Number of parameters:%d Parm list:%p New DSA:%p", LOGMSG_CLASS_VERBOSE, num_parms, r1_31bit_parms, r13_31bit_dsa);
#endif
		   for (i=0; i<num_parms; ++i) {
			   unsigned int parm = va_arg(args, unsigned int);
			   r1_31bit_parms[i] = parm;
#if DEBUG_CALL31ASM
			   tracemsg("Parameter %d: 0x%8.8x", LOGMSG_CLASS_VERBOSE, i, parm);
#endif
		   }
		   va_end(args);

		   rc = CALL31A(fn, r13_31bit_dsa, r1_31bit_parms);

#if DEBUG_CALL31ASM
		   tracemsg("Return code:%d", LOGMSG_CLASS_VERBOSE, rc);
#endif

		   if (r1_31bit_parms) free(r1_31bit_parms);
		   free(r13_31bit_dsa);

		   return rc;
	}
#endif
