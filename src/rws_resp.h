#ifndef _rws_RESP_H_
#define _rws_RESP_H_

#include "rws_recv.h"

#define CONTENT_SIZE 512
#define STATIC_PATH "./static"

typedef struct
{
  char *name;
  char *data;
  size_t size;
  int status_code;
} rws_res_body_t;

typedef struct
{
  int code;
  char* reason;
} rws_res_status_t;


typedef struct
{
  char *protocol;
  rws_res_status_t status;
  size_t content_length;
  char *content_type;
  char *data;
  size_t size;
} rws_res_head_t;

typedef struct
{
  /* data */
  char *data;
  size_t size;
} rws_res_info_t;


// GET response conetnt-type.
// File type
// HTML file:    text/html; charset=UTF-8
// CSS file:     text/css
// JS file:      application/javascript
// PNG file:     image/png
// JPEG file:    image/jpeg
// svg file:     image/svg+xml
// favicon file: image/vnd.microsoft.icon
char* rws_res_content_type(char* buf);

// Make HTML content from file.
void rws_res_body(rws_res_body_t *file_info);

// 
void rws_res_header(rws_res_head_t *header, rws_res_body_t *file_info);

//
char* rws_get_status_reason(int status_code);

//
rws_res_head_t rws_make_header(rws_res_body_t *file_info);

//
void rws_make_res_info(rws_res_info_t *res, rws_recv_info_t *recv);

#endif