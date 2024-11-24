#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ngf_resp.h"
#include "ngf_utils.h"

char* ngf_res_conent_type(char* file)
{
  if (strcmp(file, "/") == 0) return "text/html; charset=UTF-8";
  
  char *ext = strrchr(file, '.');

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