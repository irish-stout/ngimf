#ifndef _NGF_SERVER_H_
#define _NGF_SERVER_H_

void closing(int fd, void *arg, char* data);
void* connection_handle(void *arg);
void ngf_server();

#endif