#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "rws_utils.h"
#include "rws_recv.h"
#include "rws_server.h"
#include "rws_resp.h"

#define PORT 9000
// 1MB
#define REQ_BUF_SIZE 1048576

void closing(int fd, void *arg, rws_res_info_t *res, rws_recv_info_t *recv)
{
  if (arg != NULL) free(arg);
  if (res != NULL) 
  {
    free(res->data);
  }

  if (recv != NULL) 
  {
    free(recv->body);
  }
  close(fd);
}

void* connection_handle(void *arg)
{
  char buf[REQ_BUF_SIZE];
  int accept_sockfd = *((int *)arg);
  
  // Receive
  memset(buf, 0, REQ_BUF_SIZE);
  if (recv(accept_sockfd, buf, REQ_BUF_SIZE, 0) < 0)
  {
    closing(accept_sockfd, arg, NULL, NULL);
    p("Error on receive");
    return NULL;
  }

  // Get receve info
  rws_recv_info_t recv;
  rws_get_recv_info(&recv, buf);

  // make response data
  rws_res_info_t res;
  rws_make_res_info(&res, &recv);

  // Send.
  if (send(accept_sockfd, res.data, res.size, 0) < 0) 
  {
    closing(accept_sockfd, arg, &res, &recv);
    p("Error on send");
    return NULL;
  }

  closing(accept_sockfd, arg, &res, &recv);

  return NULL;
}


void rws_server()
{
  int sockfd;
  socklen_t srvlen;
  struct sockaddr_in serv_addr;
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) rws_panic("Can't open socket!");

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(PORT);

  srvlen = sizeof(serv_addr);
  
  if (bind(sockfd, (struct sockaddr *)&serv_addr, srvlen) < 0)
    rws_panic("Can't bind");

  if (listen(sockfd, SOMAXCONN) != 0) rws_panic("Can't listen");

  printf("Server is runnning at %d\n", PORT);

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