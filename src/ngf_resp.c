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
  
  return "text/html";
}


char* ngf_gen_content(char* file, char* content)
{
  FILE *fp;
  char ch;
  int index = 0, max = CONTENT_SIZE;

  fp = fopen(file, "r");
  if (fp == NULL) return "\0";
  
  content = (char *)realloc(content, max);
  memset(content, 0, max);
  while((ch = fgetc(fp)) != EOF)
  {
    if (index > max)
    {
      int i, size;
      size = index - 1;
      char tmp[size];

      memcpy(tmp, content, size);
      max *= 2;
      content = (char *)realloc(content, max);
      memset(content, 0, max);
      if (content == NULL) ngf_panic("Can't arrocate memory");
      memcpy(content, tmp, size);
    }
    content[index++] = ch;
  }
      
  fclose(fp); 

  return content;
}
