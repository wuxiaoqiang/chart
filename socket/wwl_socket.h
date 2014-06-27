#ifndef WWL_SOCKET_H
#define WWL_SOCKET_H

#include "uv.h"

typedef void (* PARSE_CB)(int status, unsigned ip, unsigned short port, void *user_data);
typedef void (* CONN_CB) (uv_stream_t *, int status, void *user_data);
typedef void (* SEND_CB) (uv_stream_t *, int status, void *user_data);
typedef void (* RECV_CB) (uv_stream_t *, int status, char *buff, int len, void *user_data);
typedef void (* CLOSE_CB) (uv_stream_t *, int status, void *user_data);

int wwl_parse_dns(char *host, unsigned short port, PARSE_CB cb, void *user_data);
int wwl_connect_by_domain(char *host, unsigned short port, CONN_CB cb, void *user_data);
int wwl_connect_by_ip(unsigned ip, unsigned short port, CONN_CB cb, void *user_data);
int wwl_send(uv_stream_t * handle, char *buff, int len, SEND_CB cb, void *user_data);
int wwl_recv(uv_stream_t * handle, char *buff, int len, RECV_CB cb, void *user_data);
int wwl_close(uv_stream_t * handle, CLOSE_CB cb, void *user_data);

#endif