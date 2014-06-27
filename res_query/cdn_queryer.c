
#include "uv.h"
#include <stdio.h>

#include "cdn_queryer.h"

#include "../test/define.h"
#include "../test/async_frame.h"


#define LOG_DEBUG(fmt, ...) printf("[%s:%d] "fmt"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)


#define _TO_STR(a) #a
#define TO_STR(a) _TO_STR(a)
// #define QUERY_CDN_DOMAIN "www.baidu.com"
#define QUERY_CDN_DOMAIN "p2p.cl.kankan.com"
#define QUERY_CDN_PORT 80
#define QUERY_CDN_PORT_STR TO_STR(QUERY_CDN_PORT)

unsigned g_query_cdn_ip;

uv_tcp_t *g_query_cdn_socket = NULL;

uv_getaddrinfo_t *g_query_cdn_dns_req;
uv_connect_t *g_query_cdn_connect_req;
uv_write_t *g_query_cdn_write_req;
uv_shutdown_t *g_query_cdn_shutdown_req;

char send_buf[2048];
int send_len;
char recv_buf[1024];
int recv_len;



void query_cdn_close_cb(uv_shutdown_t* req, int status)
{
    LOG_DEBUG("status: %d", status);

    if (status <0) 
    {
        LOG_DEBUG("error stop strerror: %s, err_name: %s", uv_strerror(status), uv_err_name(status));
    }

    free(g_query_cdn_shutdown_req);
    g_query_cdn_shutdown_req = NULL;

    free(g_query_cdn_socket);
    g_query_cdn_socket = NULL;
}

void query_cdn_close()
{
    LOG_DEBUG();
    g_query_cdn_shutdown_req = malloc(sizeof(uv_shutdown_t));
    uv_shutdown(g_query_cdn_shutdown_req, (uv_stream_t*)g_query_cdn_socket, query_cdn_close_cb);
}


int query_cdn_build_buf(char **buf, int *len, char *gcid)
{
    LOG_DEBUG();
}

void query_cdn_alloc_buf(uv_handle_t* handle,
                            size_t suggested_size,
                            uv_buf_t* buf)
{
    LOG_DEBUG("suggested_size: %d", suggested_size);
    *buf = uv_buf_init(recv_buf, sizeof(recv_buf));
}

void query_cdn_recv_cb(uv_stream_t* stream,
                           ssize_t nread,
                           const uv_buf_t* buf)
{

    if (nread <0) 
    {
        LOG_DEBUG("error stop EOF: %d, nread: %d", UV_EOF, nread);
        LOG_DEBUG("error stop strerror: %s, err_name: %s", uv_strerror(nread), uv_err_name(nread));

        uv_read_stop(stream);

        query_cdn_close();
        return;
    }

    LOG_DEBUG("nread: %d %s", (int)nread, buf->base);

    if (nread == UV_EOF);
    recv_len += nread;
    recv_buf[recv_len] = '\0';

    // if (query_cdn_handle_resp();

    // UV_EXTERN int uv_shutdown(uv_shutdown_t* req, uv_stream_t* handle, uv_shutdown_cb cb);
}

void query_cdn_send_cb(uv_write_t* req, int status)
{
    LOG_DEBUG("%d status: %d", (int)(req->data), status);

    uv_stream_t *handle = req->handle;
    free(g_query_cdn_write_req);
    g_query_cdn_write_req = NULL;

    if (status <0) 
    {
        LOG_DEBUG("error stop strerror: %s, err_name: %s", uv_strerror(status), uv_err_name(status));

        query_cdn_close();
        return;
    }

    uv_read_start(handle, query_cdn_alloc_buf, query_cdn_recv_cb);
}

void query_cdn_connect_cb(uv_connect_t* req, int status)
{
    LOG_DEBUG("%d status: %d", (int)(req->data), status);

    uv_stream_t *handle = req->handle;
    free(g_query_cdn_connect_req);
    g_query_cdn_connect_req = NULL;


    if (status <0) 
    {
        LOG_DEBUG("error stop strerror: %s, err_name: %s", uv_strerror(status), uv_err_name(status));

        free(g_query_cdn_socket);
        g_query_cdn_socket = NULL;
        //cb
        return;
    }

    g_query_cdn_write_req = malloc(sizeof(uv_write_t));
    uv_buf_t buf = uv_buf_init(send_buf, send_len);
    uv_write(g_query_cdn_write_req, handle, &buf, 1, query_cdn_send_cb);

}

void query_cdn_parse_dns_cb(uv_getaddrinfo_t *resolver, int status, struct addrinfo *res)
{
    LOG_DEBUG("status: %d", status);

    free(g_query_cdn_dns_req);
    g_query_cdn_dns_req = NULL;


    if (status <0) 
    {
        LOG_DEBUG("error stop strerror: %s, err_name: %s", uv_strerror(status), uv_err_name(status));
        //cb
    }
    else
    {
        int ip = ((struct sockaddr_in*) res->ai_addr)->sin_addr.s_addr;
        g_query_cdn_ip = ip;

        char addr[100] = {'\0'};
        uv_ip4_name((struct sockaddr_in*) res->ai_addr, addr, 100);
        int port = ((struct sockaddr_in*) res->ai_addr)->sin_port;
        int af = ((struct sockaddr_in*) res->ai_addr)->sin_family;
        LOG_DEBUG("ip: %s, port: %d, af: %d", addr, port, af);

        uv_freeaddrinfo(res);

        query_cdn_start_connect();
    }
}




int query_cdn_get_ip()
{
    LOG_DEBUG("%s %s", QUERY_CDN_DOMAIN, QUERY_CDN_PORT_STR);
    if (g_query_cdn_dns_req != NULL)
        return SUCCESS;
    g_query_cdn_dns_req = malloc(sizeof(uv_getaddrinfo_t));

    int r = uv_getaddrinfo(get_loop(), g_query_cdn_dns_req, query_cdn_parse_dns_cb, QUERY_CDN_DOMAIN, QUERY_CDN_PORT_STR, NULL);
    return r;
}


int query_cdn_start_connect()
{
    LOG_DEBUG();
        LOG_DEBUG("conn_req: %p, sock: %p", g_query_cdn_connect_req, g_query_cdn_socket);
    //if (g_query_cdn_socket == NULL)
    {
        LOG_DEBUG("conn_req: %x, sock: %p", g_query_cdn_connect_req, g_query_cdn_socket);
        g_query_cdn_socket = malloc(sizeof(uv_tcp_t));
        uv_tcp_init(get_loop(), g_query_cdn_socket);
    }

    //if (g_query_cdn_connect_req == NULL)
    {
        g_query_cdn_connect_req = malloc(sizeof(uv_connect_t));

        struct sockaddr_in dest_addr;
        // uv_ip4_addr("115.239.210.27", 80, &dest_addr);
        // uv_ip4_addr("192.168.1.5", 6211, &dest_addr);

        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(QUERY_CDN_PORT);
        // dest_addr.sin_port = 80;
        dest_addr.sin_addr.s_addr = g_query_cdn_ip;

        // g_query_cdn_connect_req->data = g_query_cdn_socket;

        char tmp[100];
        uv_ip4_name(&dest_addr, tmp, 100);


        int ret = uv_tcp_connect(g_query_cdn_connect_req, g_query_cdn_socket, (struct sockaddr *)&dest_addr, query_cdn_connect_cb);
        LOG_DEBUG("start connect %s, ret: %d, conn_req: %p, sock: %p", tmp, ret, g_query_cdn_connect_req, g_query_cdn_socket);
    }
        return 0;
}




int query_cdn_handle_resp()
{
    LOG_DEBUG();
    char *header_end = strstr(recv_buf, "\r\n\r\n");

    if (header_end == NULL)
        return -1;

    //parse http header
    // if (strstr()) return -2;
    //parse cdn_resource

    return 0;

}




int query_cdn(char *gcid, void *cb, void *arg)
{
    
    LOG_DEBUG("gcid: %s", gcid);

    sprintf(send_buf, "GET /getCdnresource_common?ver=2000&gcid=%s HTTP/1.1\r\nHost: %s:%u\r\nUser-Agent: Mozilla/4.0\r\nConnection: Keep-Alive\r\nAccept: */*\r\n\r\n",gcid, QUERY_CDN_DOMAIN ,QUERY_CDN_PORT);

    send_len = strlen(send_buf);

    LOG_DEBUG("%s", send_buf);

    
    if (g_query_cdn_ip == 0)
    {
        return query_cdn_get_ip();
    }
    
    if (g_query_cdn_socket == NULL)
    {
        return query_cdn_start_connect();
    }

    uv_buf_t buf = uv_buf_init(send_buf, send_len);
    uv_write(g_query_cdn_write_req, g_query_cdn_write_req->handle, &buf, 1, query_cdn_send_cb);
}