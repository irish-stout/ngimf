#include <string.h>
#include <stdlib.h>
#include "ngf_recv.h"
#include "ngf_utils.h"

void
ngf_get_recv_info(ngf_recv_info_t *recv, char* buf)
{
  int i = 0, j = 0, body = 0;
  char ch;
  short mode = 0;
  char str[2048];
  do
  {
    ch = buf[i++];
    if (ch == '\n')
    {
      if (mode == 0)
      {
        mode = 1;
        ngf_recv_first_line(recv, str); 
      }
      else if (ngf_is_start_of("Content-Length: ", str) == 0)
      {
        // content-length
        int len = strlen("Content-Length: ");
        char dist[32];
        strcpy(dist, &str[len]);
        recv->content_length = atoi(dist);
      }
      else if (ngf_is_start_of("content-type: ", str) == 0)
      {
        // content type
        int len = strlen("content-type: ");
        strcpy(recv->content_type, &str[len]);
      }
      else if (ngf_is_start_of("___internal-request-id: ", str) == 0)
      {
        // request_id
        int len = strlen("___internal-request-id: ");
        strcpy(recv->request_id, &str[len]);
      }
      else if ((unsigned int)buf[i] == 13)
      {
        // request body.
        i += 2;
        recv->body = (char*)realloc(recv->body, recv->content_length);
        strcpy(recv->body, &buf[i]);
        break;
      }
      
      // Reset.
      j = 0;
      memset(str, 0, 2048);
    }
    else
    {
      str[j++] = ch; 
    }
    

  }
  while (ch != '\0');  
}


void
ngf_recv_first_line(ngf_recv_info_t *recv, char* line)
{
  char ch;
  int i = 0, j = 0, mode = 0;
  do
  {
    ch = line[i++];
    if (ch == ' ' || ch == '\n' || ch == '\0') 
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
  while (ch != '\n');
}


int
ngf_is_start_of(char *target, char *src)
{
  int i, len, src_len;

  len = strlen(target);
  src_len = strlen(src);

  if (len > src_len) return -1;
  
  for (i = 0; i < len; i++)
    if (target[i] != src[i]) return -1;
  
  return 0;
}