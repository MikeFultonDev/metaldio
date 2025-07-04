#define _XOPEN_SOURCE
#define _ISOC99_SOURCE
#define _POSIX_SOURCE
#define _OPEN_SYS_FILE_EXT
#include <stdarg.h>
#include <stdio.h>
#include <_Nascii.h>

#include "msg.h"

int info(const DBG_Opts* opts, const char* fmt, ...) {
  int rc;
  va_list arg_ptr;

  if (!opts || !opts->verbose) {
    return 0;
  }

  va_start(arg_ptr, fmt);

  if (opts->info_buffer && opts->info_buffer->buffer && opts->info_buffer->size > 0) {
    DBG_MsgBuffer* msg_buf = opts->info_buffer;
    msg_buf->truncated = 0;
    
    rc = vsnprintf(msg_buf->buffer, msg_buf->size, fmt, arg_ptr);
    
    if (rc >= msg_buf->size) {
      msg_buf->truncated = 1;
    }

    if (rc > 0 && __isASCII()) {
      size_t msglen = (rc < msg_buf->size) ? rc : msg_buf->size - 1;
      if (msglen > 0) {
        __e2a_l(msg_buf->buffer, msglen);
      }
    }
  } else {
    rc = vfprintf(stdout, fmt, arg_ptr);
  }
  
  va_end(arg_ptr);
  return rc;
}

int debug(const DBG_Opts* opts, const char* fmt, ...) {
  int rc = 0;
  va_list arg_ptr;

  if (opts && opts->debug) {
    va_start(arg_ptr, fmt);
    rc = vfprintf(stderr, fmt, arg_ptr);
    va_end(arg_ptr);
  }

  return rc;
}

int errmsg(const DBG_Opts* opts, const char* fmt, ...) {
  int rc;
  va_list arg_ptr;

  va_start(arg_ptr, fmt);

  if (opts && opts->error_buffer && opts->error_buffer->buffer && opts->error_buffer->size > 0) {
    DBG_MsgBuffer* msg_buf = opts->error_buffer;
    msg_buf->truncated = 0;
    
    rc = vsnprintf(msg_buf->buffer, msg_buf->size, fmt, arg_ptr);

    if (rc >= msg_buf->size) {
      msg_buf->truncated = 1;
    }

    if (rc > 0 && __isASCII()) {
      size_t msglen = (rc < msg_buf->size) ? rc : msg_buf->size - 1;
      if (msglen > 0) {
        __e2a_l(msg_buf->buffer, msglen);
      }
    }
  } else {
    rc = vfprintf(stderr, fmt, arg_ptr);
  }

  va_end(arg_ptr);
  return rc;
}
