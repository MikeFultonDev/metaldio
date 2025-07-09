#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "metaldio.h"
#include "decb.h"
#include "dio.h"
#include "mem.h"
#include "ihadcb.h"
#include "iosvcs.h"
#include "s99.h"
#include "util.h"

/*
 * Basic Create of a FB PDSE Member:
 * - Allocate System-Generated DDName to (PDSE name passed in)
 * - Establish DCB for DDName
 * - Perform OPEN on PDSE
 * - Perform WRITE on PDSE and write a block of ASCII a's to the PDSE (0x61)
 * - Perform CHECK and NOTE on PDSE
 * - Perform STOW to create PDSE member (member name passed in) (with CCSID 819 if STOW_IFF_ON) pointing to the block written
 * - Close DCB
 * - Free DDName
 */

const struct opencb opencb_template = { 1, 0, 0, 0, 0 };
const struct stowlist_iff stowlistiff_template = { sizeof(struct stowlist_iff), 0, 0, 0, 0, 0, 0, 0 };
const struct stowlist_add stowlistadd_template = { "        ", 0, 0, 0, 0 };
const struct decb decb_template = { 0, 0x8020 };
const struct closecb closecb_template = { 1, 0, 0 };

#define ASCII_A 0x61

int main(int argc, char* argv[]) {
  struct opencb* PTR32 opencb;
  struct closecb* PTR32 closecb;
  struct ihadcb* PTR32 dcb;
  struct decb* PTR32 decb;
  int rc;
  unsigned int ttr;

  void* PTR32 block;

  union stowlist* stowlist;
  struct stowlist_add* stowlistadd;
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

  dcb = dcb_init(ddname);
  if (!dcb) {
    fprintf(stderr, "Unable to obtain storage for OPEN dcb\n");
    return 4;
  }

  /*
   * DCB set to PO, BPAM WRITE and POINT
   */
  dcb->dcbdsgpo = 1;
  dcb->dcbeodad.dcbhiarc.dcbbftek.dcbbfaln = 0x84;
  dcb->dcboflgs = dcbofuex;
  dcb->dcbmacr.dcbmacr2 = dcbmrwrt|dcbmrpt2;

  opencb = MALLOC31(sizeof(struct opencb));
  if (!opencb) {
    fprintf(stderr, "Unable to obtain storage for OPEN cb\n");
    return 4;
  }
  *opencb = opencb_template;
  opencb->dcb24 = dcb;
  opencb->mode = OPEN_OUTPUT;

  rc = OPEN(opencb);
  if (rc) {
    fprintf(stderr, "Unable to perform OPEN. rc:%d\n", rc);
    return rc;
  }

  decb = MALLOC24(sizeof(struct decb));
  if (!decb) {
    fprintf(stderr, "Unable to obtain storage for WRITE decb\n");
    return 4;
  }
  block = MALLOC24(dcb->dcbblksi);
  if (!block) {
    fprintf(stderr, "Unable to obtain storage for WRITE block\n");
    return 4;
  }
  memset(block, ASCII_A, dcb->dcbblksi);

  /*
   * Write out a 'short' block (one record less than a full block of ASCII A's)
   * by setting the dcbblksi accordingly. 
   * This is normally only on the last block being written out which
   * may be a short block.
   */
  dcb->dcbblksi -= dcb->dcblrecl;
  *decb = decb_template;
  SET_24BIT_PTR(decb->dcb24, dcb);
  decb->area = block;

  rc = WRITE(decb);
  if (rc) {
    fprintf(stderr, "Unable to perform WRITE. rc:%d\n", rc);
    return rc;
  }

  rc = CHECK(decb);
  if (rc) {
    fprintf(stderr, "Unable to perform CHECK. rc:%d\n", rc);
    return rc;
  }

  ttr = NOTE(dcb);

#define STOW_IFF_ON 1
#ifdef STOW_IFF_ON
  stowlist = MALLOC24(sizeof(struct stowlist_iff));
  stowlistadd = MALLOC24(sizeof(struct stowlist_add));
  if ((!stowlist) || (!stowlistadd)) {
    fprintf(stderr, "Unable to obtain storage for STOW\n");
    return 4;
  }
  stowlist->iff = stowlistiff_template;
  *stowlistadd = stowlistadd_template;
  memcpy(stowlistadd->mem_name, argv[2], memlen);
  STOW_SET_TTR((*stowlistadd), ttr);

  SET_24BIT_PTR(stowlist->iff.dcb24, dcb);
  stowlist->iff.type = STOW_IFF;
  stowlist->iff.direntry = stowlistadd;
  stowlist->iff.ccsid = 819;

  rc = STOW(stowlist, NULL, STOW_IFF);
  if (rc != STOW_IFF_CC_CREATE_OK) {
    fprintf(stderr, "Unable to perform STOW (Does the member already exist?). rc:%d\n", rc);
    return rc;
  }
#else
  stowlist = MALLOC24(sizeof(struct stowlist_add));
  if (!stowlist) {
    fprintf(stderr, "Unable to obtain storage for STOW\n");
    return 4;
  }
  stowlist->add = stowlistadd_template;
  memcpy(stowlist->add.mem_name, argv[2], memlen);
  STOW_SET_TTR((stowlist->add), ttr);

  rc = STOW(stowlist, dcb, STOW_A);
  if (rc != STOW_CC_OK) {
    return rc;
  }
#endif

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
