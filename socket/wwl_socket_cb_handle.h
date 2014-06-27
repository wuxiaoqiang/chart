#include "uv.h"

void connect_by_domain_cb(int status, unsigned ip, unsigned short port, void *user_data);

void parse_dns_cb(uv_getaddrinfo_t *resolver, int status, struct addrinfo *res);
void connect_cb(uv_connect_t* req, int status);
void send_cb(uv_write_t* req, int status);
void recv_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void alloc_buf_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void close_cb(uv_shutdown_t* req, int status);