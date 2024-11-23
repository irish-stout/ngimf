#include <string.h>
#include "ngf_recv.h"

ngfRecvInfo ngf_recv_info(char* buf)
{
  int i = 0, j = 0;
  char ch;
  short mode = 0; // 0: method, 1:path, 2:Protocol
  ngfRecvInfo header;
  memset(header.method, 0, 8);
  memset(header.path, 0, 256);
  memset(header.protocol, 0, 16);
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
          header.method[j++] = ch;
          break;
        case 1:
          header.path[j++] = ch;
          break;
        case 2:
          header.protocol[j++] = ch;
          break;
      }
    }
  }
  while (ch != '\n' && ch != '\0');
  
  return header;
}
