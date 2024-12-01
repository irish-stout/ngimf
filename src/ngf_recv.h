#ifndef _NGF_RECV_H_
#define _NGF_RECV_H_

#include <stdio.h>

typedef struct
{
  char method[8]; // GET, POST, PUT. DELETE, CONNECT, OPTIONS, TRACE, PATCH
  char path[2048]; // /index.html /style.css ...
  char protocol[16]; // HTTP/1.1
  char content_type[128];
  char request_id[40];
  size_t content_length;
  char *body;
} ngf_recv_info_t;

// Receive header sample
// -----------------------------
// GET /style.css HTTP/1.1
// Host: 172.22.156.143:9999
// Connection: keep-alive
// User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36
// Accept: text/css,*/*;q=0.1
// Referer: http://172.22.156.143:9999/
// Accept-Encoding: gzip, deflate
// Accept-Language: en-US,en;q=0.9,ja;q=0.8
void ngf_get_recv_info(ngf_recv_info_t *recv, char* buf);

int ngf_is_start_of(char *target, char *src);

#endif