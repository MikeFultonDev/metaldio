#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "metaldio.h"
#include "ihadcb.h"
#include "dio.h"
#include "mem.h"

const struct ihadcb dcb_template = { 0 };
const struct dcbe dcbe_template = { "DCBE", sizeof(struct dcbe) };
struct ihadcb* PTR32 dcb_init(const char* ddname)
{
  struct ihadcb* PTR32 dcb;
  struct dcbe* PTR32 dcbe;
  if (sizeof(struct ihadcb) != 0x58) {
    fprintf(stderr, "DCB should be 0x58 bytes in size but it is 0x%x bytes\n", sizeof(struct ihadcb));
    return NULL;
  }
  if (sizeof(struct dcbe) != 0x38) {
    fprintf(stderr, "DCBE should be 0x38 bytes in size but it is 0x%x bytes\n", sizeof(struct dcbe));
    return NULL;
  }

  dcb = MALLOC24(sizeof(struct ihadcb));

  if (!dcb) {
    fprintf(stderr, "Unable to obtain storage for OPEN dcb\n");
    return NULL;
  }

  dcbe = MALLOC31(sizeof(struct dcbe));

  if (!dcbe) {
    fprintf(stderr, "Unable to obtain storage for OPEN dcbe\n");
    return NULL;
  }

  *dcb = dcb_template;
  *dcbe = dcbe_template;

  dcb->dcbdcbe = dcbe;

  dcbe->usrflags = 0x80; /* QSAM buffers may be above the line */

  if (ddname) {
    size_t ddname_len = strlen(ddname);
    if (ddname_len > 8 || ddname_len == 0) {
      fprintf(stderr, "Invalid ddname passed in of length: %u. Length must be from 1 to 8 characters\n", ddname_len);
      return NULL;
    }
    memset(&dcb->dcbddnam, ' ', sizeof(dcb->dcbddnam));
    memcpy(dcb->dcbddnam, ddname, ddname_len);
  }
  dcb->dcbbufcb.dcbbufca = DCB_ADDR24_UNSET; 
  dcb->dcbiobad.dcbicqe.dcbodeb.dcbodeba = DCB_ADDR24_UNSET; 
  dcb->dcbeodad.dcbeoda = DCB_ADDR24_UNSET; 
  dcb->dcbeodad.dcbhiarc.dcbbftek.dcbbfaln = (dcbh0 | dcbh1);
  dcb->dcbeobr.dcbeobra = DCB_ADDR24_UNSET; 
  dcb->dcbgerr.dcbperr.dcbcheck.dcbchcka = DCB_ADDR24_UNSET;
  dcb->dcbsynad.dcbsyna = DCB_ADDR24_UNSET; 
  dcb->dcbcicba = DCB_ADDR24_UNSET; 
  dcb->dcbeobw = DCB_ADDR_UNSET; 
  dcb->dcbpoint = DCB_ADDR_UNSET; 

  return dcb;
}

void dcb_free(struct ihadcb* PTR32 dcb)
{
  FREE24(dcb, sizeof(struct ihadcb));
}

void dcb_fmt_dmp(const DBG_Opts* opts, struct ihadcb* PTR32 dcb)
{
  dumpstg(opts, dcb, sizeof(struct ihadcb)); 
}

