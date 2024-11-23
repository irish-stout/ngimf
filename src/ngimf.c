#include "ngf_server.h"

#include <stdio.h>
#include <stdlib.h>

int main()
{
  // FILE *fp, *fp2;
  // int i;
  // int ch;
  // unsigned long size;
  // char *buf;

  // fp = fopen("./static/image.png", "r");
  // fp2 = fopen("./static/dist.png", "w");

  // fseek(fp, 0, SEEK_END);
  // size = ftell(fp);
  // fseek(fp, 0, SEEK_SET);
  
  // buf = (char *)malloc(size);

  // fread(buf, size, 1, fp);

  // fwrite(buf, size, 1, fp2);
  // // while((ch = fgetc(fp)) != EOF)
  // // {
  // //   fputc(ch, fp2);
  // // }

  // fclose(fp);
  // fclose(fp2);
  // printf("%s\n", buf);
  
  // free(buf);
  ngf_server();
  return 0;
}