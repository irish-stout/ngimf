#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define PORT 9999
#define BUF_SIZE 2048
#define CONTENT_SIZE 64

// Exit with print error mesage. 
void panic(const char* message)
{
  perror(message);
  exit(1);
}

// Make HTML content from file.
char* html_file(const char* file, char* content)
{
  FILE *fp;
  char ch;
  int index, max;

  fp = fopen(file, "r");
  if (fp == NULL) panic("Can't open file.");

  index = 0;
  max = CONTENT_SIZE;
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
      if (content == NULL) panic("Can't arrocate memory");
      memcpy(content, tmp, size);
    }
    content[index++] = ch;
  }
      
  fclose(fp); 

  return content;
}

void main()
{
  int sockfd, accept_sockfd;
  socklen_t srvlen, clilen;
  char buf[BUF_SIZE];
  struct sockaddr_in serv_addr, cli_addr;
  char *content = malloc(CONTENT_SIZE);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) panic("Can't open socket!");

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(PORT);

  srvlen = sizeof(serv_addr);
  clilen = sizeof(cli_addr);

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    panic("Can't bind");

  if (listen(sockfd, SOMAXCONN) != 0) panic("Can't listen");

  for (;;)
  {
    accept_sockfd = accept(sockfd, (struct sockaddr *)&serv_addr, (socklen_t *)&srvlen);
    if (accept_sockfd < 0) continue;

    if (getsockname(accept_sockfd, (struct sockaddr*)&cli_addr, (socklen_t *)&clilen) < 0) continue;

    memset(buf, 0, BUF_SIZE);
    if (recv(accept_sockfd, buf, BUF_SIZE, 0) < 0) continue;

    printf("%s\n", buf);

    content = html_file("./static/index.html", content);
    char *header = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
    char res[sizeof(header) + sizeof(content)];
    sprintf(res, "%s%s", header, content);
    
    if (send(accept_sockfd, res, strlen(res), 0) < 0) continue;
    
    // free(content);
    close(accept_sockfd);
  }

  close(sockfd);
}