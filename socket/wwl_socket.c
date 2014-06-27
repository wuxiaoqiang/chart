#include "wwl_socket.h"
#include "wwl_socket_cb_handle.h"

#include "../log/log.h"
#include "../test/async_frame.h"

int wwl_parse_dns(char *host, unsigned short port, PARSE_CB cb, void *user_data)
{
    uv_getaddrinfo_t *resolver = malloc(sizeof(uv_getaddrinfo_t));

    void **ud_t = malloc(sizeof(void *)*2);

    ud_t[0] = cb;
    ud_t[1] = user_data;

    resolver->data = (void*)ud_t;

    char port_str[10];

    sprintf(port_str, "%d", port);

    int r = uv_getaddrinfo(get_loop(), resolver, parse_dns_cb, host, port_str, NULL);
    return r;
}

int wwl_connect_by_domain(char *host, unsigned short port, CONN_CB cb, void *user_data)
{
    void **ud_t = malloc(sizeof(void *)*2);

    ud_t[0] = cb;
    ud_t[1] = user_data;

    wwl_parse_dns(host,port, connect_by_domain_cb, (void*)ud_t);
}

int wwl_connect_by_ip(unsigned ip, unsigned short port, CONN_CB cb, void *user_data)
{
    LOG_DEBUG("ip: %d, port: %d", ip, port);
    uv_connect_t *req = malloc(sizeof(uv_connect_t));

    struct sockaddr_in dest_addr;

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = ip;

    void **ud_t = malloc(sizeof(void *)*2);

    ud_t[0] = cb;
    ud_t[1] = user_data;

    req->data = ud_t;

    uv_tcp_t *sock = malloc(sizeof(uv_tcp_t));
    uv_tcp_init(get_loop(), sock);


    int ret = uv_tcp_connect(req, sock, (struct sockaddr *)&dest_addr, connect_cb);
    LOG_DEBUG("ret: %d", ret);
}

int wwl_send(uv_stream_t * handle, char *buff, int len, SEND_CB cb, void *user_data)
{
    LOG_DEBUG("buff: %p, len: %d", buff, len);

    uv_write_t *req = malloc(sizeof(uv_write_t));
    uv_buf_t buf = uv_buf_init(buff, len);

    void **ud_t = malloc(sizeof(void *)*2);

    ud_t[0] = (void *)cb;
    ud_t[1] = user_data;

    req->data = ud_t;

    uv_write(req, handle, &buf, 1, send_cb);
}

int wwl_recv(uv_stream_t * handle, char *buff, int len, RECV_CB cb, void *user_data)
{
    LOG_DEBUG("buff: %p, len: %d", buff, len);
    
    void **ud_t = malloc(sizeof(void *)*5);

    ud_t[0] = (void *)cb;
    ud_t[1] = user_data;
    ud_t[2] = buff;
    ud_t[3] = (void *)len;
    ud_t[4] = 0;
    
    handle->data = ud_t;
    
    uv_read_start(handle, alloc_buf_cb, recv_cb);
}

int wwl_close(uv_stream_t * handle, CLOSE_CB cb, void *user_data)
{
    LOG_DEBUG("handle->data: %p", handle->data);
    if (handle->data)
    {
        uv_read_stop(handle);
        handle->data = 0;
    }

    void **ud_t = malloc(sizeof(void *)*2);

    ud_t[0] = (void *)cb;
    ud_t[1] = user_data;

    uv_shutdown_t *req = malloc(sizeof(uv_shutdown_t));

    req->data = ud_t;
    uv_shutdown(req, handle, close_cb);
}