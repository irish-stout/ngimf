#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ngf_resp.h"
#include "ngf_utils.h"

char* ngf_res_conent_type(char* file)
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


ngfFile ngf_res_body(char* file)
{
  FILE *fp;
  ngfFile fileInfo;

  fileInfo.size = 0;
  fileInfo.data = "";

  fp = fopen(file, "rb");
  if (fp == NULL) return fileInfo;
  
  fseek(fp, 0, SEEK_END);
  fileInfo.size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  fileInfo.data = (char *)malloc(fileInfo.size);
  memset(fileInfo.data, 0, fileInfo.size);
  fread(fileInfo.data, fileInfo.size, 1, fp);

  fclose(fp); 

  return fileInfo;
}

void ngf_res_header(ngfResHeader *header)
{
  char buf[8];
  sprintf(buf, "%d", header->contentLength);
  int size = 
    strlen(header->protocol)
    + 1
    + 3
    + 1
    + strlen(header->reason)
    + strlen("\r\n")
    + strlen("Content-type: ")
    + strlen(header->contentType)
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
    header->statusCode,
    header->reason,
    header->contentType,
    header->contentLength
  );

  header->size = strlen(header->data);
} 

ngfResHeader make_header(size_t size, char* file)
{
  ngfResHeader resHeader;
  resHeader.protocol = "HTTP/1.1";
  resHeader.statusCode = 200;
  resHeader.reason = "OK";
  resHeader.contentLength = size;
  resHeader.contentType = ngf_res_conent_type(file);
  return resHeader; 
}

ngfResHeader make_404_header(size_t size)
{
  ngfResHeader resHeader;
  resHeader.protocol = "HTTP/1.1";
  resHeader.statusCode = 404;
  resHeader.reason = "NOT FOUND";
  resHeader.contentLength = size;
  resHeader.contentType = "text/html";
  return resHeader;
}

void ngf_response(ngfResponse *res, char *file)
{
  const char* body_404 = "<html><body><h1>404 NOT FOUND</h1></body></html>";
  
  // Response Body.
  // Content (Response body)
  ngfFile fileInfo;
  fileInfo = ngf_res_body(file);
  
  // Reponse Header.
  ngfResHeader resHeader = 
    fileInfo.size > 0
    ? make_header(fileInfo.size, file)
    : make_404_header(strlen(body_404));
  ngf_res_header(&resHeader);
  
  if (fileInfo.size <= 0)
  {
    fileInfo.data = (char *)malloc(strlen(body_404));
    fileInfo.size = strlen(body_404);
  }
  
  // Response.
  res->size = resHeader.size + fileInfo.size; 
  res->data = (char *)malloc(res->size);
  memset(res->data, 0, res->size);
  memcpy(res->data, resHeader.data, resHeader.size);
  memcpy(&res->data[resHeader.size], fileInfo.data, fileInfo.size);
  free(fileInfo.data);
  free(resHeader.data);
}