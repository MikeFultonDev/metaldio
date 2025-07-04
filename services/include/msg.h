#ifndef __MSG_H__
  #define __MSG_H__ 1
  
  #include "dbgopts.h"

  /*
   * info prints informational messages (controlled by the verbose flag).
   * If opts->info_buffer is not NULL, the message is written to the provided buffer.
   * Otherwise, the message is printed to stdout.
   * The parameters that follow are standard 'printf' style parameters.
   */
  int info(const DBG_Opts* opts, const char* fmt, ...);

  /*
   * debug prints debug messages to stderr (controlled by the debug flag).
   * It does not support writing to a buffer.
   * The parameters that follow are standard 'printf' style parameters.
   */  
  int debug(const DBG_Opts* opts, const char* fmt, ...);

  /*
   * errmsg prints error messages.
   * If opts->error_buffer is not NULL, the message is written to the provided buffer.
   * Otherwise, the message is printed to stderr.
   * The parameters that follow are standard 'printf' style parameters.
   */  
  int errmsg(const DBG_Opts* opts, const char* fmt, ...);
#endif
