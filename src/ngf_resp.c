#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ngf_resp.h"
#include "ngf_utils.h"

ngf_res_status_t ngf_status_set[] = {
  { 100, "Continue" },
  { 101, "SwitchingProtocols" },
  
  { 200, "OK" },
  { 201, "Created" },
  { 202, "Accepted" },
  { 203, "Non-AuthoritativeInformation" },
  { 204, "NoContent" },
  { 205, "ResetContent" },
  { 206, "PartialContent" },
  
  { 300, "MultipleChoices" },
  { 301, "MovedPermanently" },
  { 302, "Found" },
  { 303, "SeeOther" },
  { 304, "NotModified" },
  { 305, "UseProxy" },
  { 307, "TemporaryRedirect" },
  
  { 400, "BadRequest" },
  { 401, "Unauthorized" },
  { 402, "PaymentRequired" },
  { 403, "Forbidden" },
  { 404, "NotFound" },
  { 405, "MethodNotAllowed" },
  { 406, "NotAcceptable" },
  { 407, "ProxyAuthenticationRequired" },
  { 408, "RequestTimeout" },
  { 409, "Conflict" },
  { 410, "Gone" },
  { 411, "LengthRequired" },
  { 412, "PreconditionFailed" },
  { 413, "PayloadTooLarge" },
  { 414, "URITooLong" },
  { 415, "UnsupportedMediaType" },
  { 416, "RangeNotSatisfiable" },
  { 417, "ExpectationFailed" },
  { 426, "UpgradeRequired" },

  { 500, "InternalServerError" },
  { 501, "NotImplemented" },
  { 502, "BadGateway" },
  { 503, "ServiceUnavailable" },
  { 504, "GatewayTimeout" },
  { 505, "HTTPVersionNotSupported" },
};

char* ngf_res_content_type(char* file)
{
  if (strcmp(file, "/") == 0) return "text/html; charset=UTF-8";
  
  char *ext = strrchr(&file[8], '.');

  if (strcmp(ext, ".html") == 0) return "text/html; charset=UTF-8";
  if (strcmp(ext, ".css") == 0) return "text/css";
  if (strcmp(ext, ".js") == 0) return "application/javascript";
  if (strcmp(ext, ".png") == 0) return "image/png";
  if (strcmp(ext, ".jpg") == 0) return "image/jpeg";
  if (strcmp(ext, ".gif") == 0) return "image/gif";
  
  return "application/octet-stream";
}


void
ngf_res_body(ngf_res_body_t *res_body)
{
  FILE *fp;

  res_body->size = 0;
  res_body->status_code = 200; 
  fp = fopen(res_body->name, "rb");
  if (fp == NULL) 
  {
    char *file404 = "/404.html";
    size_t resize = strlen(STATIC_PATH) + strlen(file404);
    res_body->name = realloc(res_body->name, resize);
    strcpy(res_body->name, STATIC_PATH);
    strcat(res_body->name, file404);
    fp = fopen(res_body->name, "rb");
    if (fp == NULL) return;
    res_body->status_code = 404;
  }
    
  fseek(fp, 0, SEEK_END);
  res_body->size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  res_body->data = (char *)malloc(res_body->size);
  memset(res_body->data, 0, res_body->size);
  fread(res_body->data, res_body->size, 1, fp);

  fclose(fp);
}


void
ngf_res_header(ngf_res_head_t *header, ngf_res_body_t *res_body)
{
  //
  *header = ngf_make_header(res_body);

  char buf[8];
  sprintf(buf, "%d", header->content_length);
  
  size_t size = 
    strlen(header->protocol)
    + 1
    + 3
    + 1
    + strlen(header->status.reason)
    + strlen("\r\n")
    + strlen("Content-type: ")
    + strlen(header->content_type)
    // + strlen("\r\n")
    // + strlen("Content-length: ")
    // + strlen(buf)
    + strlen("\r\n\r\n");
  header->data = (char*)malloc(size);
  memset(header->data, 0, size);

  char *base =
    "%s %d %s\r\n"
    "Content-type: %s\r\n"
    // "Content-length: %d\r\n"
    "\r\n";
    
  sprintf(header->data,
    base,
    header->protocol,
    header->status.code,
    header->status.reason,
    header->content_type
  );

  header->size = strlen(header->data);
} 


char*
ngf_get_status_reason(int status_code)
{
  int i, len;
  
  len = sizeof(ngf_status_set) / sizeof(ngf_res_status_t);
  for (i = 0; i < len; i++)
  {
    if (ngf_status_set[i].code == status_code)
      return ngf_status_set[i].reason;
  }
  
  return "";
}


ngf_res_head_t
ngf_make_header(ngf_res_body_t *res_body)
{
  ngf_res_head_t resHeader;
  resHeader.protocol = "HTTP/1.1";
  resHeader.status.code = res_body->status_code;
  resHeader.status.reason = ngf_get_status_reason(res_body->status_code);
  resHeader.content_length = res_body->size;
  resHeader.content_type = ngf_res_content_type(res_body->name);
  return resHeader; 
}


void
ngf_make_res_info(ngf_res_info_t *res, ngf_recv_info_t *recv)
{
  // file name
  char *path = recv->path[1] == '\0' ? "/index.html" : recv->path;
  
  if (strcmp(path, "/redirect") == 0) {
    char *redirect = "HTTP/1.1 308 Permanent Redirect\r\n"
                    "Location: https://www.google.com/\r\n"
                    "Content-Length: 0\r\n\r\n";
  
    res->data = (char *)malloc(strlen(redirect));
    res->data = redirect;
    res->size = strlen(redirect);
    return;
  }

  char *file = (char *)malloc(strlen(STATIC_PATH) + strlen(path));
  strcpy(file, STATIC_PATH);
  strcat(file, path);

  // Response Body.
  // Content (Response body)
  ngf_res_body_t *res_body;
  res_body->name = file;
  ngf_res_body(res_body);
  
  // Reponse Header.
  ngf_res_head_t *res_header;
  ngf_res_header(res_header, res_body);
  
  // Response.
  res->size = res_header->size + res_body->size; 
  res->data = (char *)malloc(res->size);
  memset(res->data, 0, res->size);
  memcpy(res->data, res_header->data, res_header->size);
  memcpy(&res->data[res_header->size], res_body->data, res_body->size);

  // Closing.
  free(res_body->name);
  free(res_body->data);
  free(res_header->data);
}