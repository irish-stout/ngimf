#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "ngf_utils.h"
#include "ngf_recv.h"

#define PORT 9999
#define BUF_SIZE 2048

void main()
{
  int sockfd, accept_sockfd;
  socklen_t srvlen, clilen;
  char buf[BUF_SIZE];
  struct sockaddr_in serv_addr, cli_addr;
  char *content = malloc(CONTENT_SIZE);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) ngf_panic("Can't open socket!");

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(PORT);

  srvlen = sizeof(serv_addr);
  clilen = sizeof(cli_addr);

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    ngf_panic("Can't bind");

  if (listen(sockfd, SOMAXCONN) != 0) ngf_panic("Can't listen");

  for (;;)
  {
    accept_sockfd = accept(sockfd, (struct sockaddr *)&serv_addr, (socklen_t *)&srvlen);
    if (accept_sockfd < 0) continue;

    if (getsockname(accept_sockfd, (struct sockaddr*)&cli_addr, (socklen_t *)&clilen) < 0) continue;

    // Receive
    memset(buf, 0, BUF_SIZE);
    if (recv(accept_sockfd, buf, BUF_SIZE, 0) < 0) continue;
    ngfRecvHeader recv_head;
    recv_head = ngf_recv_header_info(buf);
    
    // file name
    char file[sizeof("./static") + sizeof(recv_head.path)];
    strcpy(file, "./static");
    strcat(file, recv_head.path[1] == '\0'  ? "/index.html" : recv_head.path);
    
    // Content (Response body)
    content = ngf_gen_content(file, content);

    // 404
    if (strcmp(content, "\0") == 0)
    {
      char *res = "HTTP/1.1 404 NOT FOUND\r\n"
                  "Content-type: text/html\r\n\r\n"
                  "<html><body><h1>404 NOT FOUND</h1></body></html>";
      if (send(accept_sockfd, res, strlen(res), 0) < 0) continue;
    }
    else
    {
      // Content-Type:
      char *conetnt_type = ngf_resp_conent_type(recv_head.path);
      
      // Response header
      char header[sizeof("HTTP/1.1 200 OK\r\nContent-type: \r\n\r\n") + sizeof(conetnt_type)];
      sprintf(header, "HTTP/1.1 200 OK\r\nContent-type: %s\r\n\r\n", conetnt_type);

      // Response.
      char res[sizeof(header) + sizeof(content)];
      sprintf(res, "%s%s", header, content);
    
      // Send response.
      if (send(accept_sockfd, res, strlen(res), 0) < 0) continue;
    }
    
    close(accept_sockfd);
  }

  free(content);
  close(sockfd);
}