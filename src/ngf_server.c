#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "ngf_utils.h"
#include "ngf_recv.h"
#include "ngf_server.h"
#include "ngf_resp.h"

#define PORT 9999
#define BUF_SIZE 2048

void* connection_handle(void *arg)
{
  char buf[BUF_SIZE];
  int accept_sockfd = *((int *)arg);
  
  // Receive
  memset(buf, 0, BUF_SIZE);
  if (recv(accept_sockfd, buf, BUF_SIZE, 0) < 0)
  {
    close(accept_sockfd);
    return NULL;
  }

  ngfRecvInfo recv;
  ngf_recv_info(&recv, buf);
  
  // file name
  char *path = recv.path[1] == '\0' ? "/index.html" : recv.path;
  char file[strlen("./static") + strlen(path)];
  strcpy(file, "./static");
  strcat(file, path);

  ngfResponse res;
  ngf_response(&res, file);

  // Send.
  if (send(accept_sockfd, res.data, res.size, 0) < 0) 
  {
    close(accept_sockfd);
    return NULL;
  }

  // Closing.
  free(arg);
  free(res.data);
  close(accept_sockfd);

  return NULL;
}


void ngf_server()
{
  int sockfd;
  socklen_t srvlen;
  struct sockaddr_in serv_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) ngf_panic("Can't open socket!");

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(PORT);

  srvlen = sizeof(serv_addr);

  if (bind(sockfd, (struct sockaddr *)&serv_addr, srvlen) < 0)
    ngf_panic("Can't bind");

  if (listen(sockfd, SOMAXCONN) != 0) ngf_panic("Can't listen");

  // Main loop.
  for (;;)
  {
    struct sockaddr_in cli_addr;
    socklen_t cli_len;
    int *accept_sockfd;
    
    cli_len = sizeof(cli_addr);
    accept_sockfd = (int *)malloc(sizeof(int));
    
    // Accept 
    *accept_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
    if (*accept_sockfd < 0) continue;

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, connection_handle, (void *)accept_sockfd);
    pthread_detach(thread_id);
  }
  
  close(sockfd);

  return;
}