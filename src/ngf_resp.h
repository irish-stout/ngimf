#ifndef _NGF_RESP_H_
#define _NGF_RESP_H_

#include "ngf_recv.h"

#define CONTENT_SIZE 512
#define STATIC_PATH "./static"

typedef struct
{
  char *data;
  size_t size;
  int status_code;
} ngf_file_t;

typedef struct
{
  int code;
  char* reason;
} ngf_res_status_t;


typedef struct
{
  char *protocol;
  ngf_res_status_t status;
  size_t content_length;
  char *content_type;
  char *data;
  size_t size;
} ngf_res_head_t;

typedef struct
{
  /* data */
  char *data;
  size_t size;
} ngf_res_info_t;


// GET response conetnt-type.
// File type
// HTML file:    text/html; charset=UTF-8
// CSS file:     text/css
// JS file:      application/javascript
// PNG file:     image/png
// JPEG file:    image/jpeg
// svg file:     image/svg+xml
// favicon file: image/vnd.microsoft.icon
char* ngf_res_content_type(char* buf);

// Make HTML content from file.
void ngf_res_body(ngf_file_t *file_info, char *file);

// 
void ngf_res_header(ngf_res_head_t *header);

//
char* ngf_get_status_reason(int status_code);

//
ngf_res_head_t ngf_make_header(size_t size, char* file, int status_code);

//
void ngf_make_res_info(ngf_res_info_t *res, ngf_recv_info_t *recv);

#endif