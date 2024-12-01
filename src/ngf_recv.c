#include <string.h>
#include <stdlib.h>
#include "ngf_recv.h"
#include "ngf_utils.h"

// GET /style.css HTTP/1.1
// Host: 172.22.156.143:9999
// Connection: keep-alive
// User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36
// Accept: text/css,*/*;q=0.1
// Referer: http://172.22.156.143:9999/
// Accept-Encoding: gzip, deflate
// Accept-Language: en-US,en;q=0.9,ja;q=0.8


// GET /my.script.js HTTP/1.1
// Host: 172.22.156.143:9999
// Connection: keep-alive
// User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36
// Accept: */*
// Referer: http://172.22.156.143:9999/
// Accept-Encoding: gzip, deflate
// Accept-Language: en-US,en;q=0.9,ja;q=0.8


// GET /img/image.png HTTP/1.1
// Host: 172.22.156.143:9999
// Connection: keep-alive
// User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36
// Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8
// Referer: http://172.22.156.143:9999/
// Accept-Encoding: gzip, deflate
// Accept-Language: en-US,en;q=0.9,ja;q=0.8
//

// POST / HTTP/1.1
// Host: 172.22.156.143:9999
// Connection: keep-alive
// Content-Length: 35
// content-type: application/json
// ___internal-request-id: f4ba5306-7f81-4bfd-b551-f7a2b4ce9d5b
// Accept: */*
// Accept-Encoding: gzip, deflate
// Accept-Language: en-US,en;q=0.9,ja;q=0.8

// {
//   "sql": "select * from bland;"
// }


// POST / HTTP/1.1
// Host: 172.22.156.143:9999
// Connection: keep-alive
// Content-Length: 26
// content-type: application/x-www-form-urlencoded
// ___internal-request-id: 9cf9b498-0b25-415b-ae6f-b8d6e74d0230
// Accept: */*
// Accept-Encoding: gzip, deflate
// Accept-Language: en-US,en;q=0.9,ja;q=0.8

// name=fumiaki&age=42&age=38

// POST / HTTP/1.1
// Host: 172.22.156.143:9999
// Connection: keep-alive
// Content-Length: 35
// x-amd-emot: Test12345667
// authorization: Bearer 1230456t4
// ___internal-request-id: 444c3883-9b63-4a9a-9326-3e2930436b01
// content-type: application/json
// Accept: */*
// Accept-Encoding: gzip, deflate
// Accept-Language: en-US,en;q=0.9,ja;q=0.8

// {
//   "sql": "select * from bland;"
// }


// PUT / HTTP/1.1
// Host: 172.22.156.143:9999
// Connection: keep-alive
// Content-Length: 35
// content-type: application/json
// ___internal-request-id: ded582f5-50a0-485e-b8bf-6ba6a3abfb07
// Accept: */*
// Accept-Encoding: gzip, deflate
// Accept-Language: en-US,en;q=0.9,ja;q=0.8

// {
//   "sql": "select * from bland;"
// }


// DELETE / HTTP/1.1
// Host: 172.22.156.143:9999
// Connection: keep-alive
// ___internal-request-id: 28927eb7-f950-4ea3-9cf6-7167568cdf1e
// Accept: */*
// Accept-Encoding: gzip, deflate
// Accept-Language: en-US,en;q=0.9,ja;q=0.8

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