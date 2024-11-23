#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ngf_utils.h"

void p(char* msg)
{
  printf("%s\n", msg);
}

void ngf_panic(const char* message)
{
  perror(message);
  exit(1);
}
