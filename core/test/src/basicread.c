#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "metaldio.h"
#include "dio.h"
#include "mem.h"
#include "ihadcb.h"
#include "iosvcs.h"
#include "s99.h"
#include "iob.h"
#include "util.h"


/*
 * Basic Read of a PDS Member:
 * - Allocate DDName to PDS
 * - Establish DCB for DDName
 * - Perform OPEN on PDS
 * - Perform FIND on member
 * - Read first block of member
 * - Close DCB
 * - Free DDName
 */
const struct opencb opencb_template = { 1 };
const struct findcb findcb_template = { "        " };
const struct desp desp_template = { { { "IGWDESP ", sizeof(struct desp), 1, 0 } } };
const struct decb decb_template = { 0, 0x8080 };
const struct closecb closecb_template = { 1 };

int main(int argc, char* argv[]) {
  struct opencb* PTR32 opencb;
  struct findcb* PTR32 findcb;
  struct closecb* PTR32 closecb;
  struct ihadcb* PTR32 dcb;
  struct desp* PTR32 desp;
  struct desl* PTR32 desl;
  struct desl_name* PTR32 desl_name;
  struct desb* PTR32 desb;
  struct smde* PTR32 smde;
  struct decb* PTR32 decb;
  struct iob* PTR32 iob;
  void* PTR32 block;
  int rc;
  char* ds;
  char* mem;
  size_t memlen;

  char ddname[8+1];

  struct s99_common_text_unit dsn = { DALDSNAM, 1, 0, 0 };
  struct s99_common_text_unit dd = { DALRTDDN, 1, sizeof(DD_SYSTEM)-1, DD_SYSTEM };
  struct s99_common_text_unit stats = { DALSTATS, 1, 1, {0x8} };

  if (argc != 3) {
    fprintf(stderr, "Syntax: %s <dataset> <member>\n", argv[0]);
    return 4;
  }

  memlen = strlen(argv[2]);
  if (memlen == 0 || memlen > 8) {
    fprintf(stderr, "Member must be 1 to 8 characters long\n");
    return 4;
  }

  rc = uppercase(argv[1]);
  rc = uppercase(argv[2]);

  rc = init_dsnam_text_unit(argv[1], &dsn);
  if (rc) {
    return rc;
  }
  rc = dsdd_alloc(&dsn, &dd, &stats);
  if (rc) {
    return rc;
  }
  memcpy(ddname, dd.s99tupar, dd.s99tulng);
  ddname[dd.s99tulng] = '\0';

  rc = init_dsnam_text_unit(argv[1], &dsn);
  if (rc) {
    return 4;
  }
  rc = dsdd_alloc(&dsn, &dd, &stats);
  if (rc) {
    return 4;
  }

  mem = argv[2];
  memlen = strlen(mem);

  opencb = MALLOC31(sizeof(struct opencb));
  if (!opencb) {
    fprintf(stderr, "Unable to obtain storage for OPEN cb\n");
    return 4;
  }
  dcb = dcb_init(ddname);
  if (!dcb) {
    fprintf(stderr, "Unable to obtain storage for OPEN dcb\n");
    return 4;
  }

  /*
   * DCB set to PO, BPAM READ and POINT
   */
  dcb->dcbdsgpo = 1;
  dcb->dcboflgs = dcbofuex;
  dcb->dcbmacr.dcbmacr1 = dcbmrrd|dcbmrpt1;

  *opencb = opencb_template;
  opencb->dcb24 = dcb;

printf("Before DCB:%p DCBE:%p EODAD:%p\n", dcb, dcb->dcbdcbe, dcb->dcbdcbe->eodad);

  fprintf(stdout, "DCB:%p\n", dcb);
  dumpstg(stdout, dcb, sizeof(struct ihadcb));
  fprintf(stdout, "\nDCBE:%p\n", dcb->dcbdcbe);
  dumpstg(stdout, dcb->dcbdcbe, sizeof(struct dcbe));
  fprintf(stdout, "\n");

  rc = OPEN(opencb);
  if (rc) {
    fprintf(stderr, "Unable to perform OPEN. rc:%d\n", rc);
    return rc;
  }

  desp = MALLOC31(sizeof(struct desp));
  if (!desp) {
    fprintf(stderr, "Unable to obtain storage for DESERV\n");
    return 4;
  }
  desl = MALLOC31(sizeof(struct desl));
  if (!desl) {
    fprintf(stderr, "Unable to obtain storage for DESERV DESL\n");
    return 4;
  }
  desl_name = MALLOC31(sizeof(struct desl_name));
  if (!desl_name) {
    fprintf(stderr, "Unable to obtain storage for DESERV DESL NAME\n");
    return 4;
  }
  desl_name->desl_name_len = memlen;
  memcpy(desl_name->desl_name, mem, memlen);

  desl->desl_name_ptr = desl_name;

  /*
   * DESERV GET BYPASS_LLA LIBTYPE DCB CONN_INTENT HOLD EXT_ATTR NAME_LIST AREA
   */
  *desp = desp_template;
  desp->desp_func = desp_func_get;
  desp->desp_bypass_lla = 1;
  desp->desp_ext_attr = 1;
  desp->desp_libtype = desp_libtype_dcb;
  desp->desp_gettype = desp_gettype_name_list;
  desp->desp_conn_intent = desp_conn_intent_hold;

  /* setup DCB */
  desp->desp_dcb_ptr = dcb;

  /* setup DESERV area */
  int desb_len = sizeof(struct desb) + SMDE_NAME_MAXLEN;
  desb = MALLOC31(desb_len);
  if (!desb) {
    fprintf(stderr, "Unable to obtain storage for DESB area\n");
    return 4;
  }
  desp->desp_area_ptr = desb;
  desp->desp_area2 = desb_len;

  /* setup NAMELIST */
  /* set up DESL list of 1 entry for member to GET */

  desp->desp_name_list_ptr = desl;
  desp->desp_name_list2 = 1;

  /* call DESERV and get extended attributes */
  rc = DESERV(desp);
  if (rc) {
    fprintf(stderr, "Unable to PERFORM DESERV. rc:0x%x\n", rc);
    return 4;
  }

  smde = (struct smde* PTR32) (desp->desp_area_ptr->desb_data);
  if (smde->smde_ext_attr_off == 0) {
    fprintf(stdout, "No extended attributes for %s(%s)\n", ds, mem);
    fprintf(stdout, "SMDE Address:%p SMDE Eye-catcher %8.8s\n", smde, smde->smde_id);
  } else {
    struct smde_ext_attr* PTR32 ext_attr = (struct smde_ext_attr*) (((char*) smde) + smde->smde_ext_attr_off);
    fprintf(stdout, "CCSID: 0x%x%x last change userid: %8.8s change timestamp: 0x%llx\n",
      ext_attr->smde_ccsid[0], ext_attr->smde_ccsid[1], ext_attr->smde_userid_last_change, *((long long*) ext_attr->smde_change_timestamp));
  }

  /* Call FIND to find the start of the member */
  findcb = MALLOC31(sizeof(struct findcb));
  if (!findcb) {
    fprintf(stderr, "Unable to obtain storage for FIND macro\n");
    return 4;
  }
  *findcb = findcb_template;
  memcpy(findcb->mname, mem, memlen);

  rc = FIND(findcb, dcb);
  if (rc) {
    fprintf(stderr, "Unable to perform FIND. rc:%d\n", rc);
    return rc;
  }

  /* Establish DECB for call to READ */
  decb = MALLOC24(sizeof(struct decb));
  if (!decb) {
    fprintf(stderr, "Unable to obtain storage for READ decb\n");
    return 4;
  }
  block = MALLOC24(dcb->dcbblksi);
  if (!block) {
    fprintf(stderr, "Unable to obtain storage for READ block\n");
    return 4;
  }

  *decb = decb_template;
  SET_24BIT_PTR(decb->dcb24, dcb);
  decb->area = block;

  /* Read one block */
  rc = READ(decb);
  if (rc) {
    fprintf(stderr, "Unable to perform READ. rc:%d\n", rc);
    return rc;
  }

  

  rc = CHECK(decb);
  if (rc) {
    fprintf(stderr, "Read to end of member. rc:%d\n", rc);
    return rc;
  }

  fprintf(stdout, "Block read:%p (%d bytes)\n", block, dcb->dcbblksi);
  dumpstg(stdout, block, dcb->dcbblksi);
  fprintf(stdout, "\n");

  iob = (struct iob* PTR32) decb->stat_addr;
  fprintf(stdout, "Residual count:%d\n", iob->iobcsw.iobresct);

  /* Read another block (should fail) */
  rc = READ(decb);
  if (rc) {
    fprintf(stderr, "Unable to perform READ. rc:%d\n", rc);
    return rc;
  }

  rc = CHECK(decb);
  if (rc) {
    fprintf(stderr, "Read to end of member. rc:%d\n", rc);
    return rc;
  }

  closecb = MALLOC31(sizeof(struct closecb));
  if (!closecb) {
    fprintf(stderr, "Unable to obtain storage for CLOSE cb\n");
    return 4;
  }
  *closecb = closecb_template;
  closecb->dcb24 = dcb;
  rc = CLOSE(closecb);
  if (rc) {
    fprintf(stderr, "Unable to perform CLOSE. rc:%d\n", rc);
    return rc;
  }

  rc = FREE24(dcb, sizeof(struct ihadcb));
  if (rc) {
    fprintf(stderr, "Unable to free dcb storage. rc:%d\n", rc);
    return rc;
  }
  FREE31(opencb);

  return 0;
}
