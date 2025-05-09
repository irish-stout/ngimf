#ifndef _NGF_SERVER_H_
#define _NGF_SERVER_H_

#include "ngf_recv.h"
#include "ngf_resp.h"

void closing(int fd, void *arg, ngf_res_info_t *res, ngf_recv_info_t *recv);
void* connection_handle(void *arg);
void ngf_server();

#endif