#include <string.h>
#include <stdio.h>
#include "fmopts.h"

void init_opts(FM_Opts* opts) 
{
  opts->help = 0;
  opts->dbg.verbose = 0;
  opts->dbg.debug = 0;

  opts->dbg.error_buffer = 0;
  opts->dbg.info_buffer = 0;

  opts->fmdbg = 0;
  opts->map  = 1;
}

int process_opt(FM_Opts* opts, char* argv[], int entry)
{
  if (!strcmp(argv[entry], "-h") || !strcmp(argv[entry], "--help")) {
    opts->help = 1;
  } else if (!strcmp(argv[entry], "-v") || !strcmp(argv[entry], "--verbose")) {
    opts->dbg.verbose = 1;
  } else if (!strcmp(argv[entry], "-d") || !strcmp(argv[entry], "--debug")) {
    opts->dbg.debug = 1;
    opts->dbg.verbose = 1;
  } else if (!strcmp(argv[entry], "-D") || !strcmp(argv[entry], "--fmdbg")) {
    opts->fmdbg = 1;
  } else if (!strcmp(argv[entry], "-m") || !strcmp(argv[entry], "--mapexttollq")) {
    opts->map = 1;
  } else if (!strcmp(argv[entry], "-i") || !strcmp(argv[entry], "--ignoreext")) {
    opts->map = 0;
  } else {
    fprintf(stderr, "Option %s not recognized. Option ignored.\n", argv[entry]);
  }
  return 0;
}
