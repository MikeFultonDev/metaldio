#ifndef __DBG_OPTS_H__
  #define __DBG_OPTS_H__

  /*
   * DBG_MsgBuffer holds the configuration for capturing messages
   * into a memory buffer.
   */
  typedef struct {
    char* buffer;
    size_t size;
    int truncated:1; // set to 1 if buffer is truncated
  } DBG_MsgBuffer;

  /*
   * The DBG_Opts parameter is passed to most services and
   * can be used to control verbose and debug information
   * printed out by the services.
   */
  typedef struct {
    int verbose:1;
    int debug:1;
    DBG_MsgBuffer* info_buffer;  // If not NULL, info messages are written here.
    DBG_MsgBuffer* error_buffer; // If not NULL, error messages are written here.
  } DBG_Opts;
#endif
