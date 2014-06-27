#include "test_socket.h"
#include "wwl_socket.h"

#include "../log/log.h"

void test_conn_cb (uv_stream_t * handle, int status, void *user_data);
void test_send_cb(uv_stream_t * handle, int status, void *user_data);
void test_recv_cb (uv_stream_t *, int status, char *buff, int len, void *user_data);
void test_close_cb(uv_stream_t * handle, int status, void *user_data);

char buff[65536];


void test_conn_cb (uv_stream_t * handle, int status, void *user_data)
{
    LOG_DEBUG("status: %d", status);
    if (handle)
    {
        wwl_send(handle, "GET / HTTP/1.0\r\n\r\n", sizeof("GET / HTTP/1.0\r\n\r\n"), test_send_cb, 0);
        wwl_recv(handle, buff, 65536, test_recv_cb, 0);
        // wwl_send(handle, "GET / HTTP/1.0\r\n\r\n", sizeof("GET / HTTP/1.0\r\n\r\n"), test_send_cb, 0);
        // wwl_send(handle, "GET / HTTP/1.0\r\n\r\n", sizeof("GET / HTTP/1.0\r\n\r\n"), test_send_cb, 0);
        wwl_close(handle,test_close_cb,0);
    }
}
void test_send_cb(uv_stream_t * handle, int status, void *user_data)
{
    LOG_DEBUG("status: %d", status);
    //wwl_recv(handle, buff, 65536, test_recv_cb, 0);
}

void test_recv_cb (uv_stream_t * handle, int status, char *buff, int len, void *user_data)
{
    LOG_DEBUG("status: %d %s, %d", status, buff, len);
    wwl_close(handle,test_close_cb,0);
}

void test_close_cb(uv_stream_t * handle, int status, void *user_data)
{
    LOG_DEBUG("status: %d", status);

}


void test_socket()
{
    char domain[1024];
    int port;
    scanf("%s %d", domain, &port);
    wwl_connect_by_domain(domain, port, test_conn_cb, 0);
}