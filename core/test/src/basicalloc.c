#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "metaldio.h"
#include "dio.h"
#include "mem.h"
#include "s99.h"
#include "iosvcs.h"

/*
 * BasicAllocate of a DD name (DDPASS) to a PDS (SYS1.MACLIB) DISP=SHR
 * and then use fopen to read the first record of the member DYNALLOC.
 *
 * Note this is _different_ than if you were to just fopen the member directly
 * because fopen will use DISP=OLD if opening directly (which could fail if
 * some other task had the member open).
 */
const struct s99_rbx s99rbxtemplate = {"S99RBX",S99RBXVR,{0,1,0,0,0,0,0},0,0,0};

#define PASSLIB "SYS1.MACLIB"
#define MEMNAME "DYNALLOC"

static int pass(void) 
{
  int rc;
  FILE* fp;
  char buffer[80];
  char ddname[8+1];
  char fopen_name[80];

  struct s99_common_text_unit dsn = { DALDSNAM, 1, 0, 0 };
  struct s99_common_text_unit dd = { DALRTDDN, 1, sizeof(DD_SYSTEM)-1, DD_SYSTEM };
  struct s99_common_text_unit stats = { DALSTATS, 1, 1, {0x8} };

  rc = init_dsnam_text_unit(PASSLIB, &dsn);
  if (rc) {
    return rc;
  }
  rc = dsdd_alloc(&dsn, &dd, &stats);
  if (rc) {
    return rc;
  }
  memcpy(ddname, dd.s99tupar, dd.s99tulng);
  ddname[dd.s99tulng] = '\0';  

	printf("Allocate DD:%s to %s\n", ddname, PASSLIB);
	rc = dsdd_alloc(&dsn, &dd, &stats);

  if (rc) {
    return rc;
  }

  sprintf(fopen_name, "//DD:%s(%s)", ddname, MEMNAME);
  fp = fopen(fopen_name, "rb,type=record");
  if (!fp) {
    perror("fopen");
    return 4;
  }

  rc = fread(buffer, sizeof(buffer), 1, fp);
  if (rc != 1) {
    perror("fread");
    return 8;
  }
  printf("First record of %s(%s): <%80.80s>\n", PASSLIB, MEMNAME, buffer);

  rc = fclose(fp);
  if (rc) {
    perror("fclose");
    return 12;
  }

  return 0;
}

int main(int argc, char* argv[]) 
{
  return pass();
}
