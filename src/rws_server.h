#ifndef _rws_SERVER_H_
#define _rws_SERVER_H_

#include "rws_recv.h"
#include "rws_resp.h"

void closing(int fd, void *arg, rws_res_info_t *res, rws_recv_info_t *recv);
void* connection_handle(void *arg);
void rws_server();

#endif