#include "uv.h"

//uv cb function
void query_cdn_alloc_buf(uv_handle_t* handle,
                            size_t suggested_size,
                            uv_buf_t* buf);

void query_cdn_recv_cb(uv_stream_t* stream,
                           ssize_t nread,
                           const uv_buf_t* buf);

void query_cdn_send_cb(uv_write_t* req, int status);

void query_cdn_connect_cb(uv_connect_t* req, int status);

void query_cdn_parse_dns_cb(uv_getaddrinfo_t *resolver, int status, struct addrinfo *res);

void query_cdn_close_cb(uv_shutdown_t* req, int status);


//private used function
int query_cdn_build_buf(char **buf, int *len, char *gcid);



int query_cdn_get_ip();

int query_cdn_handle_resp();

int query_cdn_start_connect();

void query_cdn_close();

//interface
int query_cdn(char *gcid, void *cb, void *arg);
