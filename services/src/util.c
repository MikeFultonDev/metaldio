#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "metaldio.h"
#include "util.h"

int uppercase(char *string) {
  if (!string) {
    return -1;
  }

  const size_t len = strlen(string);

  for (int i = 0; i < len; i++) {
    string[i] = toupper(string[i]);
  }

  return 0;
}

int lowercase(char *string) {
  if (!string) {
    return -1;
  }

  const size_t len = strlen(string);

  for (int i = 0; i < len; i++) {
    string[i] = tolower(string[i]);
  }

  return 0;
}
