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
ngf_res_body(ngf_file_t *file_info)
{
  FILE *fp;

  file_info->size = 0;
  file_info->status_code = 200; 
  fp = fopen(file_info->name, "rb");
  if (fp == NULL) 
  {
    char *file404 = "/404.html";
    size_t resize = strlen(STATIC_PATH) + strlen(file404);
    file_info->name = realloc(file_info->name, resize);
    strcpy(file_info->name, STATIC_PATH);
    strcat(file_info->name, file404);
    fp = fopen(file_info->name, "rb");
    if (fp == NULL) return;
    file_info->status_code = 404;
  }
    
  fseek(fp, 0, SEEK_END);
  file_info->size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  file_info->data = (char *)malloc(file_info->size);
  memset(file_info->data, 0, file_info->size);
  fread(file_info->data, file_info->size, 1, fp);

  fclose(fp);
}

void ngf_res_header(ngf_res_head_t *header)
{
  char buf[8];
  
  sprintf(buf, "%d", header->content_length);
  int size = 
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

char* get_status_reason(int status_code)
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
ngf_make_header(ngf_file_t *file_info)
{
  ngf_res_head_t resHeader;
  resHeader.protocol = "HTTP/1.1";
  resHeader.status.code = file_info->status_code;
  resHeader.status.reason = get_status_reason(file_info->status_code);
  resHeader.content_length = file_info->size;
  resHeader.content_type = ngf_res_content_type(file_info->name);
  return resHeader; 
}


void
ngf_make_res_info(ngf_res_info_t *res, ngf_recv_info_t *recv)
{
  // file name
  char *path = recv->path[1] == '\0' ? "/index.html" : recv->path;
  char *file = (char *)malloc(strlen(STATIC_PATH) + strlen(path));
  strcpy(file, STATIC_PATH);
  strcat(file, path);

  // Response Body.
  // Content (Response body)
  ngf_file_t *file_info;
  file_info->name = file;
  ngf_res_body(file_info);
  
  // Reponse Header.
  ngf_res_head_t resHeader = ngf_make_header(file_info);

  ngf_res_header(&resHeader);
  
  // Response.
  res->size = resHeader.size + file_info->size; 
  res->data = (char *)malloc(res->size);
  memset(res->data, 0, res->size);
  memcpy(res->data, resHeader.data, resHeader.size);
  memcpy(&res->data[resHeader.size], file_info->data, file_info->size);

  // Closing.
  free(file_info->name);
  free(file_info->data);
  free(resHeader.data);
}