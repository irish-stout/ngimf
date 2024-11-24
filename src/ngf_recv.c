#include <string.h>
#include "ngf_recv.h"

void ngf_recv_info(ngfRecvInfo *recv, char* buf)
{
  int i = 0, j = 0;
  char ch;
  short mode = 0; // 0: method, 1:path, 2:Protocol

  memset(recv->method, 0, 8);
  memset(recv->path, 0, 256);
  memset(recv->protocol, 0, 16);
  do
  {
    ch = buf[i++];

    if (ch == ' ') 
    {
      mode++;
      j = 0;
    }
    else
    {
      switch(mode)
      {
        case 0:
          recv->method[j++] = ch;
          break;
        case 1:
          recv->path[j++] = ch;
          break;
        case 2:
          recv->protocol[j++] = ch;
          break;
      }
    }
  }
  while (ch != '\n' && ch != '\0');
  
  // return header;
}
