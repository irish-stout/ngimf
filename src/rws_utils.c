#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rws_utils.h"

void p(char* msg)
{
  printf("%s\n", msg);
}

void rws_panic(const char* message)
{
  perror(message);
  exit(1);
}
