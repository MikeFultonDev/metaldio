#define _XOPEN_SOURCE
#define _ISOC99_SOURCE
#define _POSIX_SOURCE
#define _OPEN_SYS_FILE_EXT
#include <glob.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "util.h"
#include "fm.h"
#include "fmopts.h"
#include "msg.h"
#include "filemap.h"

static int errfunc(const char *epath, int eerrno)
{
  fprintf(stderr, "%s", strerror(eerrno));
  fprintf(stderr, "Path %s is in error. No processing performed\n");
  return -1;
}

static FM_Table* allocate_table(size_t num_entries)
{
  /* 
   * This is inefficient, but allocate the table assuming one entry
   * for each extension, so guaranteed to be big enough
   */
  FM_Table* table = malloc(sizeof(FM_Table));
  if (!table) {
    return NULL;
  }
  FM_Entry* entry = calloc(num_entries, sizeof(FM_Entry));
  if (!entry) {
    free(table);
    return NULL;
  }
  table->max_size = num_entries;
  table->size = 0;
  table->entry = entry;
  return table;
}

static int cmpfn(const void* l, const void* r)
{
  FM_Entry* le = (FM_Entry*) l;
  FM_Entry* re = (FM_Entry*) r;
  return (strcmp(le->key, re->key));
}

static FM_Entry* find(const char* key, FM_Table* table)
{
  FM_Entry key_entry = { key, 0, 0 };
  FM_Entry* entry = (FM_Entry*) bsearch(&key_entry, table->entry, table->size, sizeof(FM_Entry), cmpfn);
  return entry;
}

/*
 * add: low perf implementation - should use hand-written bsearch if this is 
 * important to find the spot to insert.
 */
static FM_Entry* add(FM_Entry* entry, FM_Table* table)
{
  int i;
  for (i=0; i<table->size; ++i) {
    if (strcmp(table->entry[i].key, entry->key) > 0) {
      break;
    }
  }
  memmove(&table->entry[i+1], &table->entry[i], (table->size-i)*sizeof(FM_Entry));
  memcpy(&table->entry[i], entry, sizeof(FM_Entry));
  table->size++;

  return &table->entry[i];
}

static const char* extension(const char* path)
{
  char* dot = strrchr(path, '.');
  if (dot) {
    return dot+1;
  } else {
    return NULL;
  }
}

#define MY_PATH_MAX (1024)
glob_t* expand_file_patterns(char* argv[], int first, int last, const char* dir, glob_t* globset, const FM_Opts* opts)
{
  char file_pattern[MY_PATH_MAX+1];
  int i;
  int rc;
  for (i=first; i<last; ++i) {
    int flags;
    rc = snprintf(file_pattern, sizeof(file_pattern), "%s/%s", dir, argv[i]);
    if (rc == MY_PATH_MAX) {
      fprintf(stderr, "File pattern: <%s/%s> is too large\n", dir, argv[i]);
      return NULL;
    }
    if (i == first) {
      flags = 0;
    } else {
      flags = GLOB_APPEND;
    }
    rc = glob(file_pattern, flags, errfunc, globset);
  }

  info(&opts->dbg, "%d files to be processed\n", globset->gl_pathc);
  return globset;
}

FM_Table* create_table(glob_t* globset, const FM_Opts* opts)
{
  FM_Table* table = allocate_table(globset->gl_pathc);
  int i;

  if (!table) {
    return NULL;
  }

  /*
   * First pass is to get a count of the number of files with each
   * extension
   */
  for (i=0;i<globset->gl_pathc; ++i) {
    const char* ext = extension(globset->gl_pathv[i]);
    if (!ext || (ext[0] == '\0')) {
      continue;
    }
    FM_Entry* entry = find(ext, table);
    if (!entry) {
      FM_Entry new_entry = { ext, 0, 1 };
      add(&new_entry, table);
    } else {
      entry->count++;
    }
  }
  info(&opts->dbg, "%d extensions to be processed\n", table->size);
  for (i=0; i<table->size; ++i) {
    debug(&opts->dbg, "  [%s] has %d entries\n", table->entry[i].key, table->entry[i].count);
  }
  return table;
}

FM_Table* fill_table(glob_t* globset, FM_Table* table)
{
  int i;
  /*
   * Second pass is to allocate each file table now that the number of entries is known.
   */
  for (i=0; i<table->size; ++i) {
    int values = table->entry[i].count;
    FM_FileTable* file_table = malloc(sizeof(FM_FileTable) + (sizeof(char*)*values));
    if (!file_table) {
      return NULL;
    }
    file_table->cur_value = 0;
    table->entry[i].table = file_table;
  }
  for (i=0;i<globset->gl_pathc; ++i) {
    const char* ext = extension(globset->gl_pathv[i]);
    if (!ext || (ext[0] == '\0')) {
      continue;
    }
    FM_Entry* entry = find(ext, table);
    if (entry) {
      entry->table->values[entry->table->cur_value++] = globset->gl_pathv[i];
    } else {
      /* 
       * Error - should have been added already
       */
       fprintf(stderr, "Internal Error: file %s not in table\n", globset->gl_pathv[i]);
    }
  }
  return table;
}

const char* map_file_to_member(const char* file, char* member, const FM_Opts* opts)
{
  const char* slash = strrchr(file, '/');
  if (!slash || slash[1] == '\0') {
    return NULL;
  }

  /*
   * msf - add in length checks (truncate if option to truncate specified)
   * msf - add in checks to map invalid characters if option specified to do so
   */
  const char* dot = strrchr(file, '.');
  if (!dot || (dot < slash)) {
    dot = &file[strlen(file)];
  }
  size_t memlen = dot-slash-1;
  if (memlen > MEM_MAX) {
    return NULL;
  }

  memcpy(member, &slash[1], memlen);
  member[memlen] = 0;
  uppercase(member);

  return member;
}

const char* map_ext_to_dataset(const char* dataset_pattern, const char* ext, char* dataset, const FM_Opts* opts)
{
  /*
   * msf - add in length checks (truncate if option to truncate specified)
   * msf - add in checks to map invalid characters if option specified to do so
   */

  if (strlen(dataset_pattern) + 1 + strlen(ext) > DS_MAX) {
    return NULL;
  }

  sprintf(dataset, "%s.%s", dataset_pattern, ext);
  uppercase(dataset);

  return dataset;
}

static int cmp_mem_file_pair(const void* l, const void* r)
{
  FM_MemFilePair* lpair = (FM_MemFilePair*) l;
  FM_MemFilePair* rpair = (FM_MemFilePair*) r;

  return (strcmp(lpair->member, rpair->member));
}

int check_for_duplicate_members(glob_t* globset, const FM_Table* table, const FM_Opts* opts)
{
  FM_MemFilePair* mem_file_pair = calloc(globset->gl_pathc, sizeof(FM_MemFilePair));

  if (!mem_file_pair) {
    return 1;
  }
  
  char member_buffer[MEM_MAX+1];
  const char* member;
  int i;
  int num_members = 0;
  int rc = 0;

  /*
   * Create an array of member/file pairs, then sort it by member
   * and then loop through the sorted array and see if there are any
   * duplicates (same member name)
   */
  for (i=0; i<globset->gl_pathc; ++i) {
    member = map_file_to_member(globset->gl_pathv[i], member_buffer, opts);
    if (member) {
      strcpy(mem_file_pair[num_members].member, member);
      mem_file_pair[num_members].filename = globset->gl_pathv[i];
      num_members++;
    }
  }
  qsort(mem_file_pair, num_members, sizeof(FM_MemFilePair), cmp_mem_file_pair);
  for (i=0; i<num_members-1; ++i) {
    if (!strcmp(mem_file_pair[i].member, mem_file_pair[i+1].member)) {
      fprintf(stderr, "Error. File %s and file %s would both be copied to the same member %s\n", 
        mem_file_pair[i].filename, mem_file_pair[i+1].filename, mem_file_pair[i].member); 
      rc = 1;
    }
  }
  free(mem_file_pair);
  return rc;
}
