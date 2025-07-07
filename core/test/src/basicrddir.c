#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "metaldio.h"
#include "dio.h"
#include "mem.h"
#include "ihadcb.h"
#include "iosvcs.h"
#include "s99.h"
#include "iob.h"
#include "util.h"

/*
 * Basic Read of a PDS Directory:
 * - Allocate DDName to PDS
 * - Establish DCB for DDName
 * - Perform OPEN on PDS
 * - Read Directory via DESERV GET_ALL 
 * - Close DCB
 * - Free DDName
 */
const struct opencb opencb_template = { 1 };
const struct desp desp_template = { { { "IGWDESP ", sizeof(struct desp), 1, 0 } } };
const struct decb decb_template = { 0, 0x8080 };
const struct closecb closecb_template = { 1 };

static void print_name(FILE* stream, struct smde* PTR32 smde)
{
  struct smde_name* PTR32 name = (struct smde_name*) (((char*) smde) + smde->smde_name_off);
  char* PTR32 mem = name->smde_name_val;
  int len = name->smde_name_len;
 
  /*
   * Can use the MLT to find what alias matches which member for a PDS (not required for PDSE)
   * This will require 2 passes - one to get the MLTs and put them into a table and a second
   * to print the members out.
   */
  char* mlt = smde->smde_mltk.smde_mlt;
  fprintf(stream, "%.*s %x%x%x 0x%x", len, mem, mlt[0], mlt[1], mlt[2], smde->smde_usrd_len);
  if (smde->smde_flag_alias) {
    if (smde->smde_pname_off == 0) {
      fprintf(stream, " -> ??? ");
    } else {
      struct smde_pname* PTR32 pname = (struct smde_pname*) (((char*) smde) + smde->smde_pname_off);
      char* PTR32 pmem = pname->smde_pname_val;
      int plen = pname->smde_pname_len;
      fprintf(stream, " -> %.*s", plen, pmem);
    }
  }
}

static time_t convert_tod_to_ltime(unsigned long long tod)
{
  /*
   * Note that this conversion does not factor in leap seconds.
   * This is 'on purpose' so that it is consistent with other time
   * stamps such as the time of a zFS file, the time returned from time()
   * and other places that a C user would get the time on z/OS.
   *
   * Having consistent relative time for zFS files and PDS members being
   * reported seems more important than providing an 'accurate' time
   * for a PDS member update which is inconsistent with a time a developer
   * would get from a zFS file.
   */
  unsigned long long rawtime = tod;
  unsigned long long raw1970time = rawtime - 9048018124800000000ULL;
  double doubletime = (raw1970time >> 32);
  double doublesecs = doubletime * 1.048576;
  unsigned long rawseconds = (unsigned long) doublesecs;
  time_t ltime = (time_t) rawseconds;

  return ltime;
}

int main(int argc, char* argv[]) {
  struct opencb* PTR32 opencb;
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

  char ddname[8+1];

  struct s99_common_text_unit dsn = { DALDSNAM, 1, 0, 0 };
  struct s99_common_text_unit dd = { DALRTDDN, 1, sizeof(DD_SYSTEM)-1, DD_SYSTEM };
  struct s99_common_text_unit stats = { DALSTATS, 1, 1, {0x8} };

  if (argc != 2) {
    fprintf(stderr, "Syntax: %s <dataset>\n", argv[0]);
    return 4;
  }

  rc = uppercase(argv[1]);

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

  /*
   * DESERV GET_ALL BYPASS_LLA LIBTYPE DCB CONN_INTENT NONE EXT_ATTR NAME_LIST AREA
   */
  *desp = desp_template;
  desp->desp_func = desp_func_get_all;
  desp->desp_bypass_lla = 1;
  desp->desp_ext_attr = 1;
  desp->desp_libtype = desp_libtype_dcb;
  desp->desp_conn_intent = desp_conn_intent_none;

  /* setup DCB */
  desp->desp_dcb_ptr = dcb;

  /* setup DESERV area */
  int desb_len = sizeof(struct desb);
  desb = MALLOC31(desb_len);
  if (!desb) {
    fprintf(stderr, "Unable to obtain storage for DESB area\n");
    return 4;
  }

  desp->desp_area_ptr = desb;
  desp->desp_area2 = desb_len;
  desp->desp_areaptr_ptr = &desp->desp_area_ptr;

  /* call DESERV and get extended attributes */
  rc = DESERV(desp);
  if (rc) {
    fprintf(stderr, "Unable to PERFORM DESERV GET_ALL. rc:0x%x\n", rc);
    return 4;
  }

  struct desb* PTR32 cur_desb = desp->desp_area_ptr;
  while (cur_desb) {
    int i;
    int members = cur_desb->desb_count;
    fprintf(stdout, "Members in DESB %p: %d\n", cur_desb, members);
    /*
     * First SMDE
     */
    smde = (struct smde* PTR32) (cur_desb->desb_data);
    for (i=0; i<members; ++i) {
      print_name(stdout, smde);
      if (smde->smde_ext_attr_off != 0) {
        struct smde_ext_attr* PTR32 ext_attr = (struct smde_ext_attr*) (((char*) smde) + smde->smde_ext_attr_off);
        unsigned long long tod = *((long long *) ext_attr->smde_change_timestamp);
        time_t ltime = convert_tod_to_ltime(tod);

        fprintf(stdout, " CCSID: 0x%x%x %8.8s %s ",
          ext_attr->smde_ccsid[0], ext_attr->smde_ccsid[1], ext_attr->smde_userid_last_change, ctime(&ltime));
      } else {
        fprintf(stdout, " NO CCSID ");
      }
      if (smde->smde_usrd_off.smde_pmar_off != 0) {
        char* PTR32 user_data = (((char*) smde) + smde->smde_usrd_off.smde_pmar_off);
        int user_data_len = smde->smde_usrd_len.smde_pmar_len;
        fprintf(stdout, " User Data:%p Length:%d\n", user_data, user_data_len);
      } else {
        fprintf(stdout, " No ISPF User Data\n");
      }
      smde = (struct smde* PTR32) (((char*) smde) + smde->smde_len);
    }
    cur_desb = cur_desb->desb_next;
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

#if 0
  rc = ddfree(&dd);
  if (rc) {
    return 4;
  }
#endif

  return 0;
}
