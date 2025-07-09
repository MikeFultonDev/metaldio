#ifndef __ASMDIO_COMMON_H
  #define __ASMDIO_COMMON_H

  /*
   * Since __ptr32 is not something understood by non-Z compilers,
   * use PTR32 instead and map it to nothing if VSCODE is defined
   * (which we do in our settings for VS Code).
   */
  #ifdef VSCODE
    #define PTR32
  #else
    #define PTR32 __ptr32
  #endif
#endif