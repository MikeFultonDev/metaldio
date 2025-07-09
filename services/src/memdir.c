#define _XOPEN_SOURCE
#define _ISOC99_SOURCE
#define _POSIX_SOURCE
#define _OPEN_SYS_EXT 1
#define _OPEN_SYS_FILE_EXT 1
#define _XOPEN_SOURCE_EXTENDED 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ps.h>
#include "mem.h"
#include "memdir.h"
#include "ztime.h"
#include "metaldio.h"
#include "dio.h" 
#include "bpamio.h"
#include "bpamint.h"
#include "ispf.h"
#include "msg.h"
#include "smde.h"

void print_member(struct mstat* mstat, int print_header)
{
  if (print_header) {
    printf("MEM-ID ISPF EXT ALIAS MEM-NAME    ALIAS   EXT-ID    CCSID   EXT-CHANGED         VER MOD  CUR-LINES INIT-LINES  MOD-LINES CREATE-TIME           CHANGE-TIME  ISPF-ID\n");
  }
  char crttime_buff[4+1+2+1+2+1];                /* YYYY/MM/DD          */
  char modtime_buff[4+1+2+1+2+1+2+1+2+1+2+1];    /* YYYY/MM/DD HH:MM:SS */
  char exttime_buff[4+1+2+1+2+1+2+1+2+1+2+1];    /* YYYY/MM/DD HH:MM:SS */

  unsigned int memid = mstat->mem_id;
  char* ispf  = mstat->ispf_stats ? "Y" : "N";
  char* ext = mstat->has_ext ? "Y" : "N";
  char* alias = mstat->is_alias ? "Y" : "N";
  const char* name = (mstat->name) ? mstat->name : "NULL";
  const char* alias_name = (mstat->alias_name) ? mstat->alias_name : "NULL";;

  char* ext_id = (mstat->ext_id) ? mstat->ext_id : "NULL";
  unsigned short ccsid = mstat->ext_ccsid;
  if (mstat->has_ext) {
    struct tm* exttime = localtime(&mstat->ext_changed);
    strftime(exttime_buff, sizeof(exttime_buff), "%Y/%m/%d %H:%M:%S", exttime);
  } else {
    memcpy(exttime_buff, "none", 5);
  }
  short ver_num = mstat->ispf_version;
  short mod_num = mstat->ispf_modification;
  int cur = mstat->ispf_current_lines;
  int init = mstat->ispf_initial_lines;
  int mod = mstat->ispf_modified_lines;
  if (mstat->ispf_stats) {
    struct tm* ispf_created_time = localtime(&mstat->ispf_created);
    struct tm* ispf_changed_time = localtime(&mstat->ispf_changed);
    strftime(crttime_buff, sizeof(crttime_buff), "%Y/%m/%d", ispf_created_time);
    strftime(modtime_buff, sizeof(modtime_buff), "%Y/%m/%d %H:%M:%S", ispf_changed_time);
  } else {
    memcpy(crttime_buff, "none", 5);
    memcpy(modtime_buff, "none", 5);
  }
  char* ispf_id = (mstat->ispf_id) ? mstat->ispf_id : "NULL";

  printf("%6.6x %4s %3s %5s %8s %8s %8s %8x %21s %3d %3d %10d %10d %10d %11s %21s %8s\n", 
    memid, ispf, ext, alias, name, alias_name, ext_id, ccsid, exttime_buff, 
    ver_num, mod_num, cur, init, mod, 
    crttime_buff, modtime_buff, ispf_id);
}

static void print_members(struct mstat* mstat_arr, size_t members)
{
  printf("\n%d members to print\n", members);
  for (int i=0; i<members; ++i) {
    print_member(&mstat_arr[i], (i==0));
  }
}

static struct mstat* desp_copy_name_and_alias(struct mstat* mstat, const struct smde* PTR32 smde)
{

  char* mem_name = NULL;
  char* alias_name = NULL;

  struct smde_name* PTR32 varname = (struct smde_name*) (((char*) smde) + smde->smde_name_off);
  char* PTR32 name = varname->smde_name_val;
  int len = varname->smde_name_len;

  /*
   * If the SMDE entry is from a PDS, there will be no name offset if this entry is for 
   * an alias. 
   * However, if the SMDE entry is from a PDSE, there _will_ be a name offset if the entry
   * is for an alias.
   */
  if (smde->smde_flag_alias) {
    /*
     * Name is an alias
     */
    mstat->is_alias = 1;
    alias_name = malloc(len+1);
    if (!alias_name) {
      return NULL;
    }
    memcpy(alias_name, name, len);
    alias_name[len] = '\0';
    if (smde->smde_pname_off != 0) {
      /*
       * Pointer to underlying member name is available
       */
      struct smde_pname* PTR32 pname = (struct smde_pname*) (((char*) smde) + smde->smde_pname_off);
      char* PTR32 pmem = pname->smde_pname_val;
      int plen = pname->smde_pname_len;

      mem_name = malloc(plen+1);
      if (!name) {
        return NULL;
      }
      memcpy(mem_name, pmem, plen);
      mem_name[plen] = '\0';
    }
  } else {
    /*
     * Name is not an alias
     */
    mstat->is_alias = 0;
    mem_name = malloc(len+1);
    if (!mem_name) {
      return NULL;
    }
    memcpy(mem_name, name, len);
    mem_name[len] = '\0';
  }
  mstat->name = mem_name;
  mstat->alias_name = alias_name;

  return mstat;
}

static struct mstat* copy_extended_attributes_into_mstat(struct smde_ext_attr* ext_attr, struct mstat* mstat)
{  
  unsigned long long tod = *((long long *) ext_attr->smde_change_timestamp);
  time_t ltime = tod_to_time(tod);

  mstat->has_ext = 1;
  mstat->ext_ccsid = *((unsigned short*)(ext_attr->smde_ccsid));

  char* ext_id = malloc(USERID_LEN+1);
  if (!ext_id) {
    return NULL;
  }
  memcpy(ext_id, ext_attr->smde_userid_last_change, USERID_LEN);
  ext_id[USERID_LEN] = '\0';
  mstat->ext_id = ext_id;
  mstat->ext_changed = ltime;
  return mstat;
}

static struct mstat* copy_ispf_stats_into_mstat(const char* userdata, int userdata_len, struct mstat* mstat, const DBG_Opts* opts)
{
  if (userdata_len < ISPF_DISK_STATS_LEN) {
    debug(opts, "User data length too small. Only %d bytes\n.", userdata_len);
    mstat->ispf_stats = 0;
    return mstat;
  }

  struct ispf_stats is;
  int rc = ispf_disk_stats_to_ispf_stats(userdata, userdata_len, &is);
  if (rc) {
    mstat->ispf_stats = 0;
    debug(opts, "ISPF Stats copy failed.\n.");
    return mstat;
  }

  mstat->ispf_stats = 1;
  mstat->ispf_created = mktime(&is.create_time);
  mstat->ispf_changed = mktime(&is.mod_time);

  char* ispf_id = malloc(USERID_LEN+1);
  if (!ispf_id) {
    return NULL;
  }
  memcpy(ispf_id, is.userid, USERID_LEN);
  ispf_id[USERID_LEN] = '\0';
  mstat->ispf_id = ispf_id;

  mstat->ispf_version = is.ver_num;
  mstat->ispf_modification = is.mod_num;
  mstat->ispf_current_lines = is.curr_num_lines;
  mstat->ispf_initial_lines = is.init_num_lines;
  mstat->ispf_modified_lines = is.mod_num_lines;

  return mstat;
}

static struct mstat* smde_to_mstat(const struct smde* PTR32 smde, struct mstat* mstat, const DBG_Opts* opts)
{
  if (!mstat) {
    errmsg(opts, "NULL mstat passed to smde_to_mstat.\n");
    return NULL;
  }

  /*
   * Copy name, alias, extended attributes, and ISPF user data
   */
  const unsigned char* mlt = smde->smde_mltk.smde_mlt;
  unsigned int mem_id = (*(unsigned int*) mlt) >> 8;
  mstat->mem_id = mem_id;

  if (!desp_copy_name_and_alias(mstat, smde)) {
    errmsg(opts, "Unable to copy name and alias to mstat.\n");
    return NULL;
  }
  if (smde->smde_ext_attr_off != 0) { /* Extended attributes available */
    struct smde_ext_attr* PTR32 ext_attr = (struct smde_ext_attr*) (((char*) smde) + smde->smde_ext_attr_off);
    mstat = copy_extended_attributes_into_mstat(ext_attr, mstat);
    if (!mstat) {
      errmsg(opts, "Unable to copy extended attributes to mstat.\n");
      return NULL;
    }
    debug(opts, "Extended attribute offset information copied.\n");
  } else {
    debug(opts, "No extended attribute offset information present.\n");
  }

  if (smde->smde_usrd_off.smde_pmar_off != 0) { /* User data available */
    char* PTR32 userdata = (((char*) smde) + smde->smde_usrd_off.smde_pmar_off);
    int userdata_len = smde->smde_usrd_len.smde_pmar_len;
    mstat = copy_ispf_stats_into_mstat(userdata, userdata_len, mstat, opts);
    if (!mstat) {
      errmsg(opts, "Unable to copy ISPF Stats to mstat.\n");
      return NULL;
    }
    debug(opts, "User data offset information copied.\n");    
  } else {
    debug(opts, "No user data offset information present.\n");
  }
  return mstat;
}

static struct mstat* desp_to_mstats(const struct desp* PTR32 desp, const DBG_Opts* opts, size_t* tot_members)
{
  /*
   * Allocate array of mstat entries for all names coming from the Directory Entry Services
   * Zero out all the fields on allocation.
   */
  size_t entries = 0;

  struct desb* PTR32 cur_desb = desp->desp_area_ptr;
  while (cur_desb) {
    int sub_members = cur_desb->desb_count;
    entries += sub_members;
    cur_desb = cur_desb->desb_next;
  }

  struct mstat* mstat = calloc(entries, sizeof(struct mstat));
  if (!mstat) {
    return NULL;
  }
  *tot_members = entries;

  /*
   * Walk through the nodes and populate corresponding entries with the information
   * available.
   * Unlike the PDS directory entries, a DES directory entry will have an alias AND a name
   * or just a name (if there are no aliases for the name). 
   */
  cur_desb = desp->desp_area_ptr;
  int entry = 0;
  while (cur_desb) {
    int i;
    int sub_members = cur_desb->desb_count;

    /*
     * Walk through linked list of SMDE's and copy over SMDE info to mstat.
     */
    struct smde* PTR32 smde = (struct smde* PTR32) (cur_desb->desb_data);
    for (i=0; i<sub_members; ++i) {
      if (!smde_to_mstat(smde, &mstat[entry], opts)) {
        return NULL;
      }
      smde = (struct smde* PTR32) (((char*) smde) + smde->smde_len);
      ++entry;
    }
    cur_desb = cur_desb->desb_next;
  }
  return mstat;
}

/*
 * compare by: memid, primary member, aliases, e.g.
 * when the memid (TTR/MLT) is the same, look to the 'alias' and sort non-alias first, then look to alias name and sort by alias
 * name.
 */
static int cmp_mem_primary_alias(const void* lhs, const void* rhs)
{
  const struct mstat* l_mstat = (const struct mstat*) lhs;
  const struct mstat* r_mstat = (const struct mstat*) rhs;

  if (l_mstat->mem_id != r_mstat->mem_id) {
    return l_mstat->mem_id - r_mstat->mem_id;
  } else {
    if (l_mstat->is_alias != r_mstat->is_alias) {
      return l_mstat->is_alias - r_mstat->is_alias;
    } else {
      if (r_mstat->alias_name == NULL) {
        if (l_mstat->alias_name == NULL) {
          return 0;
        } else {
          return 1;
        }
      } else {
        /* r_mstat->alias_name != NULL */
        if (l_mstat->alias_name == NULL) {
          return -1;
        } else {
          return strcmp(r_mstat->alias_name, l_mstat->alias_name);
        }
      }
    }
  }
}

static int cmp_date(const struct mstat* l_mstat, const struct mstat* r_mstat)
{
  time_t l_chgtime;
  time_t r_chgtime;

  if (l_mstat->has_ext) {
    l_chgtime = l_mstat->ext_changed;
  } else if (l_mstat->ispf_stats) {
    l_chgtime = l_mstat->ispf_changed;
  } else {
    l_chgtime = 0;
  }
  if (r_mstat->has_ext) {
    r_chgtime = r_mstat->ext_changed;
  } else if (r_mstat->ispf_stats) {
    r_chgtime = r_mstat->ispf_changed;
  } else {
    r_chgtime = 0;
  }
  if (l_chgtime - r_chgtime > 0) {
    return 1;
  } else if (l_chgtime - r_chgtime < 0) {
    return -1;
  }

  return 0;
}

static int cmp_mem_reverse_time(const void* lhs, const void* rhs)
{
  const struct mstat* l_mstat = (const struct mstat*) lhs;
  const struct mstat* r_mstat = (const struct mstat*) rhs;

  return cmp_date(l_mstat, r_mstat);
}

static int cmp_mem_time(const void* lhs, const void* rhs)
{
  const struct mstat* l_mstat = (const struct mstat*) lhs;
  const struct mstat* r_mstat = (const struct mstat*) rhs;

  return cmp_date(l_mstat, r_mstat) * -1;
}

static int cmp_mem_reverse_name(const void* lhs, const void* rhs)
{
  const struct mstat* l_mstat = (const struct mstat*) lhs;
  const struct mstat* r_mstat = (const struct mstat*) rhs;

  return strcmp(r_mstat->name, l_mstat->name);
}

static int cmp_mem_name(const void* lhs, const void* rhs)
{
  const struct mstat* l_mstat = (const struct mstat*) lhs;
  const struct mstat* r_mstat = (const struct mstat*) rhs;

  return strcmp(l_mstat->name, r_mstat->name);
}

struct MEMDIR_Internal {
  unsigned int version;

  size_t entries;
  size_t cur;
  struct mstat head[0];
};

static void free_mstat(struct mstat* mstat, size_t entries)
{
  for (int i=0; i<entries;++i) {
    if (mstat[i].name) {
      free((char*)mstat[i].name);
    }
    if (mstat[i].alias_name) {
      free((char*)mstat[i].alias_name);
    }
    if (mstat[i].ext_id) {
      free((char*)mstat[i].ext_id);
    }
    if (mstat[i].ispf_id) {
      free((char*)mstat[i].ispf_id);
    }
  }
}

static MEMDIR* mstat_to_mdir(struct mstat* de_mstat, size_t de_members, int sort_time, int sort_reverse, const DBG_Opts* opts)
{
  if (de_members == 0) {
    return NULL;
  }

  /*
   * Sort the two arrays so they can be walked together
   */
  qsort(de_mstat, de_members, sizeof(struct mstat), cmp_mem_primary_alias);

  size_t entries = de_members;
  struct MEMDIR_Internal* mdi = malloc(sizeof(struct MEMDIR_Internal) + (entries*sizeof(struct mstat)));
  if (!mdi) {
    return NULL;
  }
  mdi->entries = entries;
  mdi->cur = 0;
  struct mstat* mdir_mstat = mdi->head;
 
  /*
   * Copy the mstat info over, dup'ing as required.
   */
  for (int i=0; i<entries; ++i) {
    mdir_mstat[i] = de_mstat[i];

    /*
     * Make sure all names are properly dup'ed so that the original
     * mstat structures can be completely freed
     */
    mdir_mstat[i].name = strdup(de_mstat[i].name);

    if (mdir_mstat[i].is_alias) {
      if (mdir_mstat[i].alias_name == NULL) {
        mdir_mstat[i].alias_name = strdup(de_mstat[i].alias_name);
      } else {
        mdir_mstat[i].alias_name = strdup(de_mstat[i].alias_name);
      }
    }
    if (mdir_mstat[i].ispf_stats && mdir_mstat[i].ispf_id != NULL) {
      mdir_mstat[i].ispf_id = strdup(de_mstat[i].ispf_id);
    }

    if (de_mstat[i].has_ext) {
      mdir_mstat[i].has_ext = 1;
      if (de_mstat[i].ext_id != NULL) {
        mdir_mstat[i].ext_id = strdup(de_mstat[i].ext_id);
      }
      mdir_mstat[i].ext_ccsid = de_mstat[i].ext_ccsid;
      mdir_mstat[i].ext_changed = de_mstat[i].ext_changed;
    }
  }

  free_mstat(de_mstat, entries);
  free(de_mstat);

  /*
   * Sort the array of mstat information
   */
  if (sort_time) {
    if (sort_reverse) {
      qsort(mdir_mstat, entries, sizeof(struct mstat), cmp_mem_reverse_time);
    } else {
      qsort(mdir_mstat, entries, sizeof(struct mstat), cmp_mem_time);
    }
  } else {
    if (sort_reverse) {
      qsort(mdir_mstat, entries, sizeof(struct mstat), cmp_mem_reverse_name);
    } else {
      qsort(mdir_mstat, entries, sizeof(struct mstat), cmp_mem_name);
    }
  }

  if (opts->debug) {
    print_members(mdir_mstat, entries);
  }
    
  return (MEMDIR*) mdi;
}

MEMDIR* openmemdir(const char* dataset, int sort_time, int sort_reverse, const DBG_Opts* opts)
{
  FM_BPAMHandle* bh;
  size_t de_members;
  if (!(bh = open_pds_for_read(dataset, opts))) {
    return NULL;
  }

  struct desp* PTR32 desp = get_desp_all(bh, opts);
  if (desp == NULL) {
    return NULL;
  }
  struct mstat* de_mstat = desp_to_mstats(desp, opts, &de_members);

  MEMDIR* memdir = mstat_to_mdir(de_mstat, de_members, sort_time, sort_reverse, opts);

  close_pds(bh, opts);

  return memdir;
}

struct mstat* readmemdir(MEMDIR* memdir, const DBG_Opts* opts)
{
  struct MEMDIR_Internal* mdi = (struct MEMDIR_Internal*) memdir;
  if (mdi->cur == mdi->entries) {
    return NULL;
  } else {
    return &mdi->head[mdi->cur++];
  }
}

int closememdir(MEMDIR* memdir, const DBG_Opts* opts)
{
  struct MEMDIR_Internal* mdi = (struct MEMDIR_Internal*) memdir;
  free_mstat(mdi->head, mdi->entries);
  free(mdi);

  return 0;
}

/*
 * Logically, writememdir_entry makes sense in memdir, but the underlying code
 * perhaps fits better in bpamio (where this request is forwarded).
 */
int writememdir_entry(FM_BPAMHandle* bh, const struct mstat* mstat, const DBG_Opts* opts)
{
  return write_member_dir_entry(mstat, bh, opts);
}

int readmemdir_entry(FM_BPAMHandle* bh, const char* mem, struct mstat* mstat, const DBG_Opts* opts)
{
  /*
   * Find the SMDE for the member and then find the mem_node for the member.
   * Merge the contents together.
   * Free up the 31-bit storage allocated for desp
   */ 
  struct desp* PTR32 desp;
  struct smde* PTR32 smde; 
    
  desp = find_desp(bh, mem, opts);
  if (!desp) {
    errmsg(opts, "Unable to find member %s.\n", mem);
    return 4;
  }

  smde = (struct smde* PTR32) (desp->desp_area_ptr->desb_data);

  if (!smde_to_mstat(smde, mstat, opts)) {
    errmsg(opts, "Unable to copy statistics from smde for %s.\n", mem);
    return 4;
  }

  free_desp(desp, opts);

  if (opts->debug) {
    fprintf(stdout, "Print out member entry %s.\n", mem);
    print_member(mstat, 1);
  }
  return 0;
}

struct mstat* create_mstat(struct mstat* mstat, char* userid, const char* alias_name, const char* name, const void* ttr, int num_lines, int ccsid, const DBG_Opts* opts)
{
  time_t cur_time;
  unsigned int mem_id = (*(unsigned int*) ttr) >> 8;
  mstat->mem_id = mem_id;
  if (alias_name != NULL) {
    mstat->is_alias = 1;
    mstat->alias_name = alias_name;
  } else {
    mstat->is_alias = 0;
    mstat->name = name;
  }

  time ( &cur_time );

  mstat->ispf_stats = 1;

  mstat->ispf_created = cur_time;
  mstat->ispf_changed = cur_time;

  if (userid == NULL) {
    char default_userid[USERID_LEN+1];
    __getuserid(default_userid, USERID_LEN);
    mstat->ispf_id = strdup(default_userid);
  } else {
    mstat->ispf_id = strdup(userid);
  }

  mstat->ispf_version = 1;
  mstat->ispf_modification = 1;
  mstat->ispf_current_lines = num_lines;
  mstat->ispf_initial_lines = num_lines;
  mstat->ispf_modified_lines = num_lines;

  mstat->has_ext = 1;
  mstat->ext_ccsid = ccsid;
  mstat->ext_id = strdup(mstat->ispf_id);
  mstat->ext_changed = cur_time;

  return mstat;
}
