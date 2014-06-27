#include "wwl_socket.h"
#include "wwl_socket_cb_handle.h"

#include "../log/log.h"


void connect_by_domain_cb(int status, unsigned ip, unsigned short port, void *user_data)
{
    LOG_DEBUG("ip: %u, port: %u, ud: %p", ip, port, user_data);

    void **ud_t = user_data;

    CONN_CB cb_t = ud_t[0];
    void *user_data_t = ud_t[1];
    free(ud_t);

    if (status<0)
    {
        cb_t(0, status, user_data_t);
    }
    else
    {
        wwl_connect_by_ip(ip, port, cb_t, user_data_t);
    }

}

void parse_dns_cb(uv_getaddrinfo_t *resolver, int status, struct addrinfo *res)
{
    LOG_DEBUG("status: %d", status);

    void **ud_t = resolver->data;

    PARSE_CB cb_t = ud_t[0];
    void *user_data_t = ud_t[1];
    free(ud_t);

    free(resolver);

    unsigned ip = 0;
    unsigned short port = 0;

    if (status <0) 
    {
        LOG_DEBUG("error stop strerror: %s, err_name: %s", uv_strerror(status), uv_err_name(status));
    }
    else
    {
        ip = ((struct sockaddr_in*) res->ai_addr)->sin_addr.s_addr;
        port = ((struct sockaddr_in*) res->ai_addr)->sin_port;
    }

    LOG_DEBUG("ip: %u, port: %u", ip, port);
    uv_freeaddrinfo(res);
    cb_t(status, ip, ntohs(port), user_data_t);
}

void connect_cb(uv_connect_t* req, int status)
{
    LOG_DEBUG("status: %d", status);
    if (status <0) 
    {
        LOG_DEBUG("error stop strerror: %s, err_name: %s", uv_strerror(status), uv_err_name(status));
    }
    void **ud_t = req->data;

    CONN_CB cb_t = ud_t[0];
    void *user_data_t = ud_t[1];
    free(ud_t);

    cb_t(req->handle, status, user_data_t);
}

void send_cb(uv_write_t* req, int status)
{
    LOG_DEBUG("status: %d", status);
    if (status <0) 
    {
        LOG_DEBUG("error stop strerror: %s, err_name: %s", uv_strerror(status), uv_err_name(status));
    }
    void **ud_t = req->data;

    SEND_CB cb_t = ud_t[0];
    void * user_data_t = ud_t[1];
    free(ud_t);

    cb_t(req->handle, status, user_data_t);
}


void recv_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
    void **ud_t = stream->data;

    LOG_DEBUG("nread: %d %s, EOF_VALUE: %d", (int)nread, buf->base, UV_EOF);
    if (nread <0) 
    {
        LOG_DEBUG("error stop strerror: %s, err_name: %s", uv_strerror(nread), uv_err_name(nread));

        uv_read_stop(stream);


        RECV_CB cb_t = ud_t[0];
        void *user_data_t = ud_t[1];
        char * buff = ud_t[2];
        int len = (int)ud_t[3];
        int recved = (int)ud_t[4];
        free(ud_t);

        stream->data = 0;

        cb_t(stream, nread, buff, recved, user_data_t);

        return;
    }

    int tmp = (int)ud_t[4];
    tmp += nread;
    ud_t[4] = (void*)tmp;
}

void alloc_buf_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
    LOG_DEBUG("suggested_size: %d", suggested_size);

    void **ud_t = handle->data;

    char * buff = ud_t[2];
    int len = (int)ud_t[3];
    int recved = (int)ud_t[4];

    LOG_DEBUG("buff: %p, len: %d, recved: %d", buff, len, recved);
    *buf = uv_buf_init(buff+recved, len-recved);
}

void close_cb(uv_shutdown_t* req, int status)
{
    LOG_DEBUG("status: %d", status);

    if (status <0) 
    {
        LOG_DEBUG("error stop strerror: %s, err_name: %s", uv_strerror(status), uv_err_name(status));
    }


    void **ud_t = req->data;

    CLOSE_CB cb_t = ud_t[0];
    void * user_data_t = ud_t[1];
    free(ud_t);

    cb_t(req->handle, status, user_data_t);

    free(req->handle);
    free(req);
}