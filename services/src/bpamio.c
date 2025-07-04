#define _XOPEN_SOURCE
#define _ISOC99_SOURCE
#define _POSIX_SOURCE
#define _OPEN_SYS_FILE_EXT 1
#define _OPEN_SYS_EXT 1
#define _XOPEN_SOURCE_EXTENDED 1

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/ps.h>

#include "asmdio.h"

#include "util.h"
#include "dio.h"
#include "mem.h"
#include "iosvcs.h"
#include "msg.h"
#include "bpamio.h"
#include "bpamint.h"
#include "findcb.h"
#include "ispf.h"
#include "ztime.h"
#include "memdir.h"
#include "iob.h"

static int bpam_open(FM_BPAMHandle* handle, int mode, const DBG_Opts* opts)
{
  struct ihadcb* PTR32 dcb;
  struct opencb* PTR32 opencb;
  struct decb* PTR32 decb;
  void* PTR32 block;

  const struct opencb opencb_template = { 1, 0, 0, 0, 0 };
  int rc;

  dcb = dcb_init(handle->ddname);
  if (!dcb) {
    errmsg(opts, "Unable to obtain storage for OPEN dcb\n");
    return 4;
  }

  /*
   * DCB set to PO, BPAM INPUT|OUTPUT and POINT
   */
  dcb->dcbeodad.dcbhiarc.dcbbftek.dcbbfaln = 0x84;
  dcb->dcboflgs = dcbofuex;

  switch (mode) {
    case OPEN_INPUT:
      dcb->dcbmacr.dcbmacr1 = dcbmrrd|dcbmrpt1;
      break;
    case OPEN_OUTPUT:
      dcb->dcbmacr.dcbmacr2 = dcbmrwrt|dcbmrpt2;
      break;
    default:
      errmsg(opts, "bpam_open function only supports INPUT and OUTPUT. %d specified\n", mode);
      return 4;
  }

  opencb = MALLOC31(sizeof(struct opencb));
  if (!opencb) {
    errmsg(opts, "Unable to obtain storage for OPEN cb\n");
    return 4;
  }
  *opencb = opencb_template;
  opencb->dcb24 = dcb;
  opencb->mode = mode;

  rc = OPEN(opencb);
  if (rc) {
    errmsg(opts, "Unable to perform OPEN. rc:%d\n", rc);
    return rc;
  }

  if (!dcb->dcbdsgpo) {
    errmsg(opts, "Dataset is not a PDSE.\n");
    return 4;
  }

  decb = MALLOC24(sizeof(struct decb));
  if (!decb) {
    errmsg(opts, "Unable to obtain storage for WRITE decb\n");
    return 4;
  }
  block = MALLOC24(dcb->dcbblksi);
  if (!block) {
    errmsg(opts, "Unable to obtain storage for WRITE block\n");
    return 4;
  }

  handle->dcb = dcb;
  handle->opencb = opencb;
  handle->decb = decb;
  handle->block = block;
  handle->block_size = dcb->dcbblksi;
  handle->bytes_used = 0;

  handle->pdsstart_ttr = NOTE(dcb);
  handle->pdsstart_ttr_known = 1;

  return 0;
}

static int bpam_open_read(FM_BPAMHandle* handle, const DBG_Opts* opts)
{
  return bpam_open(handle, OPEN_INPUT, opts);
}

static int bpam_open_write(FM_BPAMHandle* handle, const DBG_Opts* opts)
{
  return bpam_open(handle, OPEN_OUTPUT, opts);
}

static void validate_var_block(FM_BPAMHandle* bh, const DBG_Opts* opts)
{
  if (!opts->debug) {
    return;
  }

  char* block_char = (char*) (bh->block);
  unsigned short* block_hw = (unsigned short*) (bh->block);
  unsigned short block_size = block_hw[0];

  debug(opts, "Validate Block: Block Size: %d\n", block_size);
  char* next_rec_start = &(((char*)block_hw)[4]);
  int line_num = 1;
  while ((next_rec_start - block_char) < block_size) {
    unsigned short rec_length = *((unsigned short*) next_rec_start); 
    debug(opts, "Record %d Length: %d\n", line_num++, rec_length);
    if (rec_length < 4) {
      errmsg(opts, "Unexpected record length. Validation Failed.\n");
      exit(4);
    }
    next_rec_start = &next_rec_start[rec_length];
  }
  if (next_rec_start - block_char != block_size) {
    errmsg(opts, "Total record length did not match block size: (%d,%d)\n", next_rec_start - block_char, block_size);
    exit(4);
  }
}

/*
 * Read block.
 */
static int read_block(FM_BPAMHandle* bh, const DBG_Opts* opts)
{
  const struct decb decb_template = { 0, 0x8020 };
  *(bh->decb) = decb_template;
  SET_24BIT_PTR(bh->decb->dcb24, bh->dcb);
  bh->decb->area = bh->block;

  /* Read one block */
  int rc = READ(bh->decb);
  if (rc) {
    errmsg(opts, "Unable to perform READ. rc:%d\n", rc);
    return rc;
  }
  rc = CHECK(bh->decb);
  debug(opts, "RC:%d from CHECK on READ of block.\n", rc);

  /*
   * Initialize record offset information so that next_record can be called.
   */
  bh->next_record_start = NULL;
  bh->next_record_len = 0;

  return rc;
}
/*
 * Read a record. Return non-zero when no next record.
 * Fixed Block Short blocks need special consideration: https://tech.mikefulton.ca/BlockLengthReadDetermination
 */

static int next_record(FM_BPAMHandle* bh, const DBG_Opts* opts)
{
  char* block_char = (char*) (bh->block);
  unsigned short* block_hw = (unsigned short*) (bh->block);

  if (bh->next_record_start == NULL) {
    /*
     * Skip over header of block
     */
    if (bh->dcb->dcbexlst.dcbrecfm & dcbrecv) {
      bh->next_record_start = &block_char[4];
    } else {
      bh->next_record_start = block_char;
    }
  } else {
    bh->next_record_start = &bh->next_record_start[bh->next_record_len];
  }

  if (bh->dcb->dcbexlst.dcbrecfm & dcbrecv) {
    unsigned short block_size = block_hw[0];
    if (bh->next_record_start >= &block_char[block_size]) {
      return 0;
    }

    /*
     * If variable record length, then length is in first half word.
     * Also note the length includes the 4 byte prefix as well.
     */
    unsigned short* vreclenp = (unsigned short*) (bh->next_record_start);
    bh->next_record_len = (*vreclenp - 4);
    bh->next_record_start += 4;
  } else {
    /*
     * The residual count indicates how many pad bytes are at the end
     * of the last block of a fixed block member. This needs to be
     * subtracted from the block size to determine if you are at the 
     * end of the block.
     */
    struct iob* PTR32 iob = (struct iob* PTR32) bh->decb->stat_addr;
    unsigned short residual = iob->iobcsw.iobresct;
    unsigned short block_size = bh->dcb->dcbblksi;
    unsigned short bytes_in_block = block_size - residual;

    if (bh->next_record_start >= &block_char[bytes_in_block]) {
      return 0;
    }

    /*
     * The record is fixed length, so the length of the record is always
     * the same.
     */
    bh->next_record_len = bh->dcb->dcblrecl;
  }
  return 1;
}

/*
 * Write out a block. Returns 0 if successful, non-zero otherwise
 */
static int write_block(FM_BPAMHandle* bh, const DBG_Opts* opts)
{
  if (bh->bytes_used == 0) {
    return 0; /* empty block */
  }

  const struct decb decb_template = { 0, 0x8020 };
  *(bh->decb) = decb_template;
  SET_24BIT_PTR(bh->decb->dcb24, bh->dcb);
  bh->decb->area = bh->block;

  debug(opts, "FB:%c VB:%c bytes_used:%d block_size:%d\n", 
    (bh->dcb->dcbexlst.dcbrecfm & dcbrecf) ? 'Y' : 'N', 
    (bh->dcb->dcbexlst.dcbrecfm & dcbrecv) ? 'Y' : 'N', 
    bh->bytes_used, 
    bh->block_size
  );
  if (bh->dcb->dcbexlst.dcbrecfm & dcbrecv) {
    /*
     * Specify the block size for the variable length records 
     */
    unsigned short* halfword = (unsigned short*) (bh->block);
    halfword[0] = bh->bytes_used;  /* size of block */
    halfword[1] = 0;
    halfword[3] = 0;
    bh->dcb->dcbblksi = bh->block_size;

    validate_var_block(bh, opts);
    debug(opts, "(Block Write) First Record length:%d bytes used:%d\n", halfword[2], halfword[0]);

  } else if (bh->dcb->dcbexlst.dcbrecfm & dcbrecf) { 
    bh->dcb->dcbblksi = bh->bytes_used;
  } else {
    errmsg(opts, "Not sure how to write a block that is not recv or recf\n");
    return 4;
  }

  int rc = WRITE(bh->decb);
  if (rc) {
    errmsg(opts, "Unable to perform WRITE. rc:%d\n", rc);
    return rc;
  }
  rc = CHECK(bh->decb);
  if (rc) {
    errmsg(opts, "Unable to perform CHECK on WRITE. rc:%d\n", rc);
    return rc;
  }  

  if (!bh->memstart_ttr_known) {
    bh->memstart_ttr = NOTE(bh->dcb);
    bh->memstart_ttr_known = 1;
  }

  bh->bytes_used = 0;
  return 0;
}
static int can_add_record_to_block(FM_BPAMHandle* bh, size_t rec_len)
{
  int line_length;
  if (bh->dcb->dcbexlst.dcbrecfm & dcbrecv) {
    const int hdr_size = sizeof(unsigned int);
    line_length = rec_len + hdr_size;
  } else if (bh->dcb->dcbexlst.dcbrecfm & dcbrecf) {
    line_length = bh->dcb->dcblrecl;
  }
  int rc = (line_length + bh->bytes_used <= bh->block_size);
  return rc;
}

/*
 * copy_record_to_block returns 'truncated' (non-zero if record truncated, otherwise zero)
 */
static int copy_record_to_block(FM_BPAMHandle* bh, unsigned short usr_rec_len, const char* rec, const DBG_Opts* opts)
{
  int truncated = 0;
  debug(opts, "Add Record of length: %d bytes. Block bytes used: %d\n", usr_rec_len, bh->bytes_used);
 
  const int BDW_SIZE = 4;
  const int RDW_SIZE = 4;

  unsigned short disk_len;
  unsigned short rec_len;

  char* block_char = (char*) (bh->block);
  int rec_hdr_size;
  if (bh->dcb->dcbexlst.dcbrecfm & dcbrecv) {
    /*
     * Variable format
     */    
    unsigned short* next_rec;
    rec_hdr_size = BDW_SIZE;
    if (bh->bytes_used == 0) {
      /*
       * First word is block length - clear it to 0 for now
       */
      unsigned int* start = (unsigned int*) (bh->block);
      start[0] = 0;
      bh->bytes_used += BDW_SIZE;
    }
    /*
     * Determine logical and disk record length
     */
    next_rec = (unsigned short*) (&block_char[bh->bytes_used]);
    disk_len = usr_rec_len + RDW_SIZE;
    if (disk_len > bh->dcb->dcblrecl) {
      rec_len = bh->dcb->dcblrecl - RDW_SIZE;
      disk_len = bh->dcb->dcblrecl;
      truncated = 1;
    } else {
      rec_len = usr_rec_len;
    }
    
    next_rec[0] = disk_len;
    
    next_rec[1] = 0;
    bh->bytes_used += RDW_SIZE;
    debug(opts, "Disk Record length:%d bytes used:%d\n", next_rec[0], bh->bytes_used);
  } else {
    /*
     * Fixed format
     */
    rec_hdr_size = 0;
    if (usr_rec_len > bh->dcb->dcblrecl) {
      disk_len = bh->dcb->dcblrecl;
      rec_len = disk_len;
      truncated = 1;
    } else {
      disk_len = usr_rec_len;
      rec_len = disk_len;
    }
  }
  if (truncated) {
    info(opts, "Long record encountered on line %d and truncated. Maximum %d expected but record is %d bytes\n", bh->line_num, bh->dcb->dcblrecl, usr_rec_len);
  }

  debug(opts, "Copy data to disk from offset: %d for %d bytes. disk_len:%d rec_len:%d\n", bh->bytes_used, rec_len, disk_len, rec_len);
  
  memcpy(&block_char[bh->bytes_used], rec, rec_len);
  bh->bytes_used += rec_len; 
 
  if (bh->dcb->dcbexlst.dcbrecfm & dcbrecf) {
    /*
     *  If the record is FIXED, then pad the record out with blanks
     */
    int pad_length = bh->dcb->dcblrecl - rec_len;
    debug(opts, "Pad record %d by %d blanks\n", bh->line_num, pad_length);
    if (pad_length > 0) {
      memset(&block_char[bh->bytes_used], ' ', pad_length); /* msf - choose ASCII or EBCDIC space based on ccsid */
    }
    bh->bytes_used += pad_length;
  }
  return truncated;
}

/*
 * flush any records to disk, if it is a partial block.
 * Non-zero return code indicates failure to flush
 */
int flush(FM_BPAMHandle* bh, const DBG_Opts* opts)
{
  int rc = write_block(bh, opts);
  if (rc) {
    return -1;
  } else {
    return 0;
  }
}

ssize_t write_record(FM_BPAMHandle* bh, size_t rec_len, const char* rec, const DBG_Opts* opts)
{
  /*
   * Batch up records until there is a full block and write it out 
   */
  ssize_t rc;
  if (can_add_record_to_block(bh, rec_len)) {
    int truncated = copy_record_to_block(bh, rec_len, rec, opts);
    bh->line_num++;
    rc = 0;
  } else {
    rc = write_block(bh, opts);
  }
  return rc;
}

size_t record_length(FM_BPAMHandle* bh, const DBG_Opts* opts)
{
  return bh->dcb->dcblrecl;
}

ssize_t read_record_direct(FM_BPAMHandle* bh, char** rec, size_t* rec_len, const DBG_Opts* opts)
{
  /*
   * See if we need to read another block
   */
  if ((bh->line_num == 0) || !next_record(bh, opts)) {
    ssize_t rc = read_block(bh, opts);
    if (rc) {
      return -1;
    }
    next_record(bh, opts);
    bh->line_num++;
  }
  debug(opts, "Read record direct. Start: %p Length:%d\n", bh->next_record_start, bh->next_record_len);
  *rec = bh->next_record_start;
  *rec_len = bh->next_record_len;
  return bh->next_record_len;
}

ssize_t read_record(FM_BPAMHandle* bh, size_t max_rec_len, char* rec, const DBG_Opts* opts)
{

  size_t internal_len;
  char* internal_rec;
  ssize_t rec_len = read_record_direct(bh, &internal_rec, &internal_len, opts);
  if (rec_len < 0) {
    return rec_len;
  }
  if (rec_len > max_rec_len) {
    errmsg(opts, "record length is too large. max:%d received: %d.\n", max_rec_len, rec_len);
    return -1;
  }
  memcpy(rec, internal_rec, rec_len);

  return rec_len;
}

const struct desp desp_template = { { { "IGWDESP ", sizeof(struct desp), 1, 0 } } };
const struct decb decb_template = { 0, 0x8080 };
struct desp* PTR32 get_desp_all(const FM_BPAMHandle* bh, const DBG_Opts* opts)
{
  struct desp* PTR32 desp;
  struct desl* PTR32 desl;
  struct desl_name* PTR32 desl_name;
  struct desb* PTR32 desb;
  struct decb* PTR32 decb;
  struct smde* PTR32 smde;
  int rc;

  desp = MALLOC31(sizeof(struct desp));
  if (!desp) {
    errmsg(opts, "Unable to obtain storage for DESERV\n");
    return NULL;
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
  desp->desp_dcb_ptr = bh->dcb;

  /* setup DESERV area */
  int desb_len = sizeof(struct desb);
  desb = MALLOC31(desb_len);
  if (!desb) {
    errmsg(opts, "Unable to obtain storage for DESB area\n");
    return NULL;
  }

  desp->desp_area_ptr = desb;
  desp->desp_area2 = desb_len;
  desp->desp_areaptr_ptr = &desp->desp_area_ptr;

  /* call DESERV and get extended attributes */
  rc = DESERV(desp);
  if (rc) {
    errmsg(opts, "Unable to PERFORM DESERV GET_ALL. rc:0x%x\n", rc);
    return NULL;
  }

  return desp;
}

struct desp* PTR32 init_desp(const FM_BPAMHandle* bh, const char* mem, const DBG_Opts* opts)
{

  struct desp* PTR32 desp;
  struct desl* PTR32 desl;
  struct desl_name* PTR32 desl_name;
  struct desb* PTR32 desb;
  struct decb* PTR32 decb;
  int rc;
  size_t memlen;

  memlen = strlen(mem);

  desp = MALLOC31(sizeof(struct desp));
  if (!desp) {
    errmsg(opts, "Unable to obtain storage for DESERV\n");
    return NULL;
  }
  desl = MALLOC31(sizeof(struct desl));
  if (!desl) {
    errmsg(opts, "Unable to obtain storage for DESERV DESL\n");
    return NULL;
  }
  desl_name = MALLOC31(sizeof(struct desl_name));
  if (!desl_name) {
    errmsg(opts, "Unable to obtain storage for DESERV DESL NAME\n");
    return NULL;
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
  desp->desp_dcb_ptr = bh->dcb;

  /* setup DESERV area */
  int desb_len = sizeof(struct desb) + SMDE_NAME_MAXLEN;
  desb = MALLOC31(desb_len);
  if (!desb) {
    errmsg(opts, "Unable to obtain storage for DESB area\n");
    return NULL;
  }
  desp->desp_area_ptr = desb;
  desp->desp_area2 = desb_len;

  /* setup NAMELIST */
  /* set up DESL list of 1 entry for member to GET */

  desp->desp_name_list_ptr = desl;
  desp->desp_name_list2 = 1;

  return desp;
}

int find_member(FM_BPAMHandle* bh, const char* mem, const DBG_Opts* opts)
{
  const struct findcb findcb_template = { "        " };
  size_t memlen = strlen(mem);

  /* Call FIND to find the start of the member */
  struct findcb* PTR32 findcb = MALLOC31(sizeof(struct findcb));
  if (!findcb) {
    errmsg(opts, "Unable to obtain storage for FIND macro\n");
    return 4;
  }
  *findcb = findcb_template;
  memcpy(findcb->mname, mem, memlen);

  int rc = FIND(findcb, bh->dcb);

  free(findcb);

  if (rc) {
    errmsg(opts, "Unable to perform FIND. rc:%d\n", rc);
    return rc;
  }

  /*
   * Clear the next record code so that we can know to read a block at the start of next_record
   */
  bh->next_record_len = 0;
  bh->next_record_start = NULL;

  return 0;
}

int read_member_dir_entry(struct desp* PTR32 desp, const DBG_Opts* opts)
{
  /* call DESERV and get extended attributes */
  int rc = DESERV(desp);
  if (rc) {
    errmsg(opts, "Unable to PERFORM DESERV. rc:0x%x\n", rc);
    return 4;
  }

  struct smde* PTR32 smde = (struct smde* PTR32) (desp->desp_area_ptr->desb_data);
  debug(opts, "Extended attributes for %.*s\n", desp->desp_name_list_ptr->desl_name_ptr->desl_name_len, desp->desp_name_list_ptr->desl_name_ptr->desl_name);
  if (smde->smde_ext_attr_off == 0) {
    debug(opts, "(no extended attributes) SMDE Address:%p SMDE Eye-catcher %8.8s\n", smde, smde->smde_id);
  } else {
    struct smde_ext_attr* PTR32 ext_attr = (struct smde_ext_attr*) (((char*) smde) + smde->smde_ext_attr_off);
    debug(opts, "CCSID: 0x%x%x last change userid: %8.8s change timestamp: 0x%llx\n",
      ext_attr->smde_ccsid[0], ext_attr->smde_ccsid[1], ext_attr->smde_userid_last_change, *((long long*) ext_attr->smde_change_timestamp));
  }

  return 0;
}

const struct stowlist_add stowlistadd_template = { "        ", 0, 0, 0, 0 };
static void add_mem_stats(struct stowlist_add* PTR32 sla, const struct mstat* mstat, unsigned int ttr, const DBG_Opts* opts)
{
  char userid[8+1] = "        "; 
  *sla = stowlistadd_template;
  memcpy(sla->mem_name, mstat->name, strlen(mstat->name)); 
  STOW_SET_TTR((*sla), ttr);

  unsigned int userdata_len = sizeof(struct ispf_disk_stats)/2; /* number of halfwords of ISPF statistics */
  sla->c = userdata_len;

  struct ispf_disk_stats ids = { 0 };
  __getuserid(userid, sizeof(userid));

  ids.extended = 1;
  time_t t;
  struct tm * ltime;

  time ( &t );
  ltime = localtime ( &t );

  tm_to_pdjd(&ids.mod_century, ids.pd_mod_julian, ltime);
  ids.pd_mod_hours = d_to_pd(ltime->tm_hour, 0);
  ids.pd_mod_minutes = d_to_pd(ltime->tm_min, 0);
  ids.pd_mod_seconds = d_to_pd(ltime->tm_sec, 0);
  memcpy(&ids.userid, userid, sizeof(userid)-1);

  if (mstat->ispf_stats) {
    struct tm* create_time;
    create_time = localtime(&mstat->ispf_created);

    tm_to_pdjd(&ids.create_century, ids.pd_create_julian, create_time);

    ids.ver_num = mstat->ispf_version;
    ids.mod_num = mstat->ispf_modification;

    ids.full_curr_num_lines = mstat->ispf_current_lines; 
    ids.full_init_num_lines = mstat->ispf_initial_lines; 
    ids.full_mod_num_lines = mstat->ispf_modified_lines;

    if (mstat->ispf_current_lines < SHRT_MAX) {
      ids.curr_num_lines = mstat->ispf_current_lines;
    }
    if (mstat->ispf_initial_lines < SHRT_MAX) {
      ids.init_num_lines = mstat->ispf_initial_lines;
    }
    if (mstat->ispf_modified_lines < SHRT_MAX) {
      ids.mod_num_lines = mstat->ispf_modified_lines;
    }
  } else {
    tm_to_pdjd(&ids.create_century, ids.pd_create_julian, ltime);
  }

  memcpy(sla->user_data, &ids, sizeof(struct ispf_disk_stats));
}

static int write_pds_member_dir_entry(struct ihadcb* PTR32 dcb, const char* member, struct stowlist_add* stowlistadd, const DBG_Opts* opts)
{
  union stowlist* stowlist;

  stowlist = MALLOC24(sizeof(struct stowlist_add));
  if (!stowlist) {
    errmsg(opts, "Unable to obtain storage for STOW\n");
    return 4;
  }
  stowlist->add = *stowlistadd;

  int rc = STOW(stowlist, dcb, STOW_R);
  if (rc == STOW_REPLACE_MEMBER_DOES_NOT_EXIST || rc == STOW_CC_OK) {
    debug(opts, "Member %s successfully replaced\n", member);
    rc = 0;
  } else {
    errmsg(opts, "STOW REPLACE failed for PDS member %s with rc:%d\n", member, rc);
  }

  return rc;
}

static int update_pdse_member_dir_entry(FM_BPAMHandle* bh, const char* member, union stowlist* stowlist, const DBG_Opts* opts)
{
  int rc;
  struct desp* PTR32 desp = init_desp(bh, member, opts);
  if (!read_member_dir_entry(desp, opts)) {
    /*
     * Try again and perform an UPDATE
     */
    struct smde* PTR32 smde = (struct smde* PTR32) (desp->desp_area_ptr->desb_data);
    struct smde_ext_attr* PTR32 ext_attr = (struct smde_ext_attr*) (((char*) smde) + smde->smde_ext_attr_off);
    memcpy(stowlist->iff.timestamp, ext_attr->smde_change_timestamp, STOWLIST_IFF_TIMESTAMP_LEN);
    rc = STOW(stowlist, NULL, STOW_IFF);
    if (rc != STOW_CC_OK) {
      errmsg(opts, "STOW failed for PDSE member update of %s with rc:%d\n", member, rc);
    }
  }
  return rc;
}

int write_member_dir_entry(const struct mstat* mstat, FM_BPAMHandle* bh, const DBG_Opts* opts)
{
  const struct stowlist_iff stowlistiff_template = { sizeof(struct stowlist_iff), 0, 0, 0, 0, 0, 0, 0 };
  union stowlist* stowlist;
  struct stowlist_add* stowlistadd;
  stowlist = MALLOC24(sizeof(struct stowlist_iff));
  stowlistadd = MALLOC24(sizeof(struct stowlist_add));
  int rc;

  if ((!stowlist) || (!stowlistadd)) {
    errmsg(opts, "Unable to obtain storage for STOW\n");
    return 4;
  }

  add_mem_stats(stowlistadd, mstat, bh->memstart_ttr, opts);

  stowlist->iff = stowlistiff_template;

  SET_24BIT_PTR(stowlist->iff.dcb24, bh->dcb);
  stowlist->iff.type = STOW_IFF;
  stowlist->iff.direntry = stowlistadd;
  stowlist->iff.ccsid = mstat->ext_ccsid;

  /*
   * Assume the is a PDSE and we can STOW with IFF.
   * Also assume the member does not exist yet.
   */
  rc = STOW(stowlist, NULL, STOW_IFF);
  switch (rc) {
    case STOW_IFF_CC_CREATE_OK:
      rc = 0;
      break;
    case STOW_IFF_CC_PDS_UPDATE_UNSUPPORTED:
      debug(opts, "Member %s is in a PDS - do a STOW and not a STOW_IFF\n", mstat->name);
      free(stowlist);
      rc = write_pds_member_dir_entry(bh->dcb, mstat->name, stowlistadd, opts);
      break;
    case STOW_IFF_CC_MEMBER_EXISTS:
      debug(opts, "Member %s already exists - update it.\n", mstat->name);
      rc = update_pdse_member_dir_entry(bh, mstat->name, stowlist, opts);
      break;
    default:
      errmsg(opts, "STOW failed for member %s create. rc:0x%x\n", mstat->name, rc);
      break;
  }
  return rc;
}

static int alloc_pds(const char* dataset, FM_BPAMHandle* bh, const DBG_Opts* opts)
{
  struct s99_common_text_unit dsn = { DALDSNAM, 1, 0, 0 };
  struct s99_common_text_unit dd = { DALRTDDN, 1, sizeof(DD_SYSTEM)-1, DD_SYSTEM };
  struct s99_common_text_unit stats = { DALSTATS, 1, 1, { DALSTATS_SHR } };

  int rc = init_dsnam_text_unit(dataset, &dsn);
  if (rc) {
    return rc;
  }
  rc = dsdd_alloc(&dsn, &dd, &stats);
  if (rc) {
    return rc;
  }

  /*
   * Copy system generated DD name into passed in handle
   */
  memcpy(bh->ddname, dd.s99tupar, dd.s99tulng);
  bh->ddname[dd.s99tulng] = '\0';

  debug(opts, "Allocated DD:%s to %s\n", bh->ddname, dataset);

  return 0;
}

FM_BPAMHandle* open_pds_for_read(const char* dataset, const DBG_Opts* opts)
{
  FM_BPAMHandle * bh = calloc(sizeof(FM_BPAMHandle), 1);
  if (!bh) {
    return bh;
  }
  int rc = alloc_pds(dataset, bh, opts);
  if (!rc) {
    rc = bpam_open_read(bh, opts);
  }
  if (rc) {
    return NULL;
  } else {
    return bh;
  }
}

FM_BPAMHandle* open_pds_for_write(const char* dataset, const DBG_Opts* opts)
{
  FM_BPAMHandle*bh = calloc(sizeof(FM_BPAMHandle), 1);
  if (!bh) {
    return bh;
  }
  int rc = alloc_pds(dataset, bh, opts);
  if (!rc) {
    rc = bpam_open_write(bh, opts);
  }
  if (rc) {
    return NULL;
  } else {
    return bh;
  }
}

int close_pds(FM_BPAMHandle* bh, const DBG_Opts* opts)
{
  const struct closecb closecb_template = { 1, 0, 0 };
  struct closecb* PTR32 closecb;
  int rc;

  struct s99_common_text_unit dd = { DUNDDNAM, 1, 0, 0 };
  int ddname_len = strlen(bh->ddname);
  dd.s99tulng = ddname_len;
  memcpy(dd.s99tupar, bh->ddname, ddname_len);

  closecb = MALLOC31(sizeof(struct closecb));
  if (!closecb) {
    errmsg(opts, "Unable to obtain storage for CLOSE cb\n");
    return 4;
  }
  *closecb = closecb_template;
  closecb->dcb24 = bh->dcb;

  rc = CLOSE(closecb);
  if (rc) {
    errmsg(opts, "Unable to perform CLOSE. rc:%d\n", rc);
    return rc;
  }

  rc = ddfree(&dd);
  debug(opts, "Free DD:%s\n", bh->ddname);

  free(bh);

  return rc;
}

struct desp* PTR32 find_desp(FM_BPAMHandle* bh, const char* memname, const DBG_Opts* opts)
{
  const struct desp desp_template = { { { "IGWDESP ", sizeof(struct desp), 1, 0 } } };
  size_t memlen = strlen(memname);

  /*
   * Allocate the data structures and call DESERVE GET
   */
  struct desp* PTR32 desp;
  struct desl* PTR32 desl;
  struct desl_name* PTR32 desl_name;
  struct desb* PTR32 desb;
  struct smde* PTR32 smde;
  struct decb* PTR32 decb;

  desp = MALLOC31(sizeof(struct desp));
  if (!desp) {
    errmsg(opts, "Unable to obtain storage for DESERV\n");
    return NULL;
  }
  desl = MALLOC31(sizeof(struct desl));
  if (!desl) {
    errmsg(opts, "Unable to obtain storage for DESERV DESL\n");
    return NULL;
  }
  desl_name = MALLOC31(sizeof(struct desl_name));
  if (!desl_name) {
    errmsg(opts, "Unable to obtain storage for DESERV DESL NAME\n");
    return NULL;
  }
  desl_name->desl_name_len = memlen;
  memcpy(desl_name->desl_name, memname, memlen);

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
  desp->desp_dcb_ptr = bh->dcb;

  /* setup DESERV area */
  int desb_len = sizeof(struct desb) + SMDE_NAME_MAXLEN;
  desb = MALLOC31(desb_len);
  if (!desb) {
    errmsg(opts, "Unable to obtain storage for DESB area\n");
    return NULL;
  }
  desp->desp_area_ptr = desb;
  desp->desp_area2 = desb_len;

  /* setup NAMELIST */
  /* set up DESL list of 1 entry for member to GET */
  desp->desp_name_list_ptr = desl;
  desp->desp_name_list2 = 1;

  /* call DESERV and get extended attributes */
  int rc = DESERV(desp);
  if (rc) {
    errmsg(opts, "Unable to PERFORM DESERV. rc:0x%x\n", rc);
    return NULL;
  }

  return desp;
}

void free_desp(struct desp* PTR32 desp, const DBG_Opts* opts)
{
  free(desp->desp_area_ptr);
  free(desp->desp_name_list_ptr->desl_name_ptr);
  free(desp->desp_name_list_ptr);
  free(desp);
}

static char* PTR32 ispf_rname(const char* ds, const char* mem, const DBG_Opts* opts)
{
  unsigned int rname_len = strlen(ds) + strlen(mem);

  if (rname_len > 44+8) {
    errmsg(opts, "Invalid dataset or member name passed to ENQ/DEQ %s(%s)\n", ds, mem);
    return NULL;
  }

  char* PTR32 rname;
  rname = MALLOC31(52+1);
  if (!rname) {
    errmsg(opts, "Unable to obtain storage for ENQ/DEQ\n");
    return NULL;
  }
  sprintf(rname, "%-44s%-8s", ds, mem);

  return rname;
}

static char* PTR32 ispf_qname(const char* qn, const DBG_Opts* opts)
{
  unsigned int qname_len = strlen(qn);

  if (qname_len > 8) {
    errmsg(opts, "Invalid queue name passed to ENQ/DEQ %s\n", qn);
    return NULL;
  }

  char* PTR32 qname;
  qname = MALLOC31(8+1);
  if (!qname) {
    errmsg(opts, "Unable to obtain storage for ENQ/DEQ\n");
    return NULL;
  }
  sprintf(qname, "%-8s", qn);

  return qname;
}

int enq_dataset_member(const char* ds, const char* wmem, const DBG_Opts* opts) 
{
  char* PTR32 rname = ispf_rname(ds, wmem, opts);
  char* PTR32 qname = ispf_qname("SPFEDIT", opts);

  if (!rname || !qname) {
    return 4;
  }
  int rc = SYEXENQ(qname, rname, strlen(rname));
  free(rname);
  free(qname);
  return rc;
}

int deq_dataset_member(const char* ds, const char* wmem, const DBG_Opts* opts) 
{
  char* PTR32 rname = ispf_rname(ds, wmem, opts);
  char* PTR32 qname = ispf_qname("SPFEDIT", opts);

  if (!rname || !qname) {
    return 4;
  }
  int rc = SYEXDEQ(qname, rname, strlen(rname));
  free(rname);
  free(qname);
  return rc;
}
