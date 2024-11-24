#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "ngf_utils.h"
#include "ngf_recv.h"
#include "ngf_server.h"
#include "ngf_resp.h"

#define PORT 9999
#define BUF_SIZE 2048

void ngf_server()
{
  int sockfd, accept_sockfd;
  socklen_t srvlen, clilen;
  char buf[BUF_SIZE];
  struct sockaddr_in serv_addr, cli_addr;

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

  // Main loop.
  for (;;)
  {
    // Accept 
    accept_sockfd = accept(sockfd, (struct sockaddr *)&serv_addr, (socklen_t *)&srvlen);
    if (accept_sockfd < 0) continue;

    // Socket name.
    if (getsockname(accept_sockfd, (struct sockaddr*)&cli_addr, (socklen_t *)&clilen) < 0) continue;

    // Receive
    memset(buf, 0, BUF_SIZE);
    if (recv(accept_sockfd, buf, BUF_SIZE, 0) < 0) continue;
    ngfRecvInfo recv;
    recv = ngf_recv_info(buf);
    
    // file name
    char *path = recv.path[1] == '\0' ? "/index.html" : recv.path;
    char file[strlen("./static") + strlen(path)];
    strcpy(file, "./static");
    strcat(file, path);
    
    // Content (Response body)
    ngfFile fileInfo;
    fileInfo = ngf_res_body(file);

    // 404
    if (fileInfo.size == 0)
    {
      char *res = "HTTP/1.1 404 NOT FOUND\r\n"
                  "Content-type: text/html\r\n\r\n"
                  "<html><body><h1>404 NOT FOUND</h1></body></html>";
      if (send(accept_sockfd, res, strlen(res), 0) < 0) continue;
    }
    else
    {
      // Reponse Header.
      ngfResHeader resHeader;
      resHeader.protocol = "HTTP/1.1";
      resHeader.statusCode = 200;
      resHeader.reason = "OK";
      resHeader.contentLength = fileInfo.size;
      resHeader.contentType = ngf_res_conent_type(recv.path);
      ngf_res_header(&resHeader);

      // Response.
      int resSize = resHeader.size + fileInfo.size; 
      char res[resSize];
      memset(res, 0, resSize);
      memcpy(res, resHeader.data, resHeader.size);
      memcpy(&res[resHeader.size], fileInfo.data, fileInfo.size);
      
      // Send.
      if (send(accept_sockfd, res, sizeof(res), 0) < 0) continue;
      
      // Closing.
      free(resHeader.data);
      free(fileInfo.data);
    }
    
    close(accept_sockfd);
  }

  
  close(sockfd);
}