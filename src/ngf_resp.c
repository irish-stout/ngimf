#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ngf_resp.h"
#include "ngf_utils.h"

char* ngf_resp_conent_type(char* file)
{
  char tmp[16];
  int i = 0, j = 0;
  char ch;
  bool isFound = false;
  do
  {
    ch = file[i++];
    if (isFound && ch != '\0' && ch != ' ')
    {
      tmp[j++] = ch;
    }
    else if (ch == '.') isFound = true;
  }
  while (ch != '\0');
  
  char ext[j];
  memcpy(ext, tmp, j);
  
  if (strcmp(ext, "html") == 0) return "text/html; charset=UTF-8";
  if (strcmp(ext, "css") == 0) return "text/css";
  if (strcmp(ext, "js") == 0) return "application/javascript";
  if (strcmp(ext, "png") == 0) return "image/png";
  if (strcmp(ext, "jpg") == 0) return "image/jpeg";
  if (strcmp(ext, "gif") == 0) return "image/gif";
  
  return "text/html";
}


char* ngf_gen_content(char* file, char* content)
{
  FILE *fp;
  char ch;
  int index = 0;
  unsigned int size;

  fp = fopen(file, "rb");
  if (fp == NULL) return "\0";
  
  // initialize.
  content = (char *)realloc(content, CONTENT_SIZE);
  memset(content, 0, CONTENT_SIZE);
  
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  // size set.
  content = (char *)realloc(content, size);
  memset(content, 0, size);

  // Read file.
  while ((ch = fgetc(fp)) != EOF)
    content[index++] = ch;

  fclose(fp); 

  return content;
}
