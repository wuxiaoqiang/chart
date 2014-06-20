#include "http_request.h"

#include "../log/log.h"
#include "../test/async_frame.h"

int kv_map_cmp(struct KV_PAIR *a, struct KV_PAIR *b)
{
    return strcmp(a->key, b->key);
}

RB_GENERATE(KV_MAP_RB, KV_PAIR, node, kv_map_cmp);





int set_request_info(HTTP_REQUEST *request, char *host, unsigned ip, unsigned short port, char *headline)
{
    if (host)
        strcpy(request->_host, host);
    request->_ip = ip;
    request->_port = port;
    strcpy(request->_headline, headline);

    return 0;
}

int set_cb_info(HTTP_REQUEST *request, HTTP_REQUEST_CB *cb)
{
    request->cb = *cb;

    return 0;
}

int set_request_head(HTTP_REQUEST *request, const char *key, const char *value)
{
    if (key == NULL)
        return -1;

    struct KV_PAIR *pair = malloc(sizeof(struct KV_PAIR));

    pair->key = (char *)key;

    struct KV_PAIR *find_res = RB_FIND(KV_MAP_RB, &request->headers, pair);

    if (find_res)
    {
        if (value != NULL)
        {
            free(find_res->value);
            pair->value = malloc(strlen(value) + 1);
            strcpy(pair->value, value);
        }
        else
        {
            RB_REMOVE(KV_MAP_RB, &request->headers, find_res);
            free(find_res->key);
            free(find_res->value);
            free(find_res);
        }
    }
    else if (value != NULL)
    {
        pair->key = malloc(strlen(key) + 1);
        strcpy(pair->key, key);
        pair->value = malloc(strlen(value) + 1);
        strcpy(pair->value, value);
        RB_INSERT(KV_MAP_RB, &request->headers, pair);
    }
    return 0;
}

int start_request(HTTP_REQUEST *request, const char *data)
{
    ;
}

const char *get_reponse_head(HTTP_REQUEST *request, char *key)
{
    if(request->reponse.result == -1)
        return NULL;

    struct KV_PAIR pair;
    pair.key = key;
    pair.value = NULL;

    struct KV_PAIR *find_res = RB_FIND(KV_MAP_RB, &request->reponse.headers, &pair);

    if (find_res)
        return find_res->value;

    return NULL;
}



//////////////////////////////////////////////////////////////////////////////////

void get_ip_cb(uv_getaddrinfo_t *resolver, int status, struct addrinfo *res)
{
    LOG_DEBUG("status: %d", status);

    HTTP_REQUEST *request = (HTTP_REQUEST *)resolver->data;
    free(resolver);

    if (status <0) 
    {
        LOG_DEBUG("error stop strerror: %s, err_name: %s", uv_strerror(status), uv_err_name(status));
        //cb
    }
    else
    {
        int ip = ((struct sockaddr_in*) res->ai_addr)->sin_addr.s_addr;
        request->_ip = ip;

        char addr[100] = {'\0'};
        uv_ip4_name((struct sockaddr_in*) res->ai_addr, addr, 100);
        int port = ((struct sockaddr_in*) res->ai_addr)->sin_port;
        int af = ((struct sockaddr_in*) res->ai_addr)->sin_family;
        LOG_DEBUG("ip: %s, port: %d, af: %d", addr, port, af);

        uv_freeaddrinfo(res);

        start_connect(request);
    }
}



void recv_cb(uv_stream_t* stream,
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
void connect_cb(uv_connect_t* req, int status)
{
    LOG_DEBUG("%d status: %d", (int)(req->data), status);

    uv_stream_t *handle = req->handle;
    HTTP_REQUEST *request = req->data;
    free(req);


    if (status <0) 
    {
        LOG_DEBUG("error stop strerror: %s, err_name: %s", uv_strerror(status), uv_err_name(status));

        // free(g_query_cdn_socket);
        // g_query_cdn_socket = NULL;
        //cb
        return;
    }

    char *buff = malloc(4096);

    uv_write_t *write_req = malloc(sizeof(uv_write_t));
    write_req->data = request;
    uv_buf_t buf = uv_buf_init(buff, 4096);
    uv_write(write_req, handle, &buf, 1, send_cb);
}


void send_cb(uv_write_t* req, int status)
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


void close_cb(uv_shutdown_t* req, int status)
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


static int get_ip(HTTP_REQUEST *request)
{
    uv_getaddrinfo_t *dns_req = malloc(sizeof(uv_getaddrinfo_t));

    dns_req->data = request;

    int r = uv_getaddrinfo(get_loop(), dns_req, get_ip_cb, request->_host, "80", NULL);
    return r;
}


int start_connect(HTTP_REQUEST *request)
{
    LOG_DEBUG();
    LOG_DEBUG("conn_req: %p, sock: %p", g_query_cdn_connect_req, g_query_cdn_socket);
    //if (g_query_cdn_socket == NULL)
    {
        LOG_DEBUG("conn_req: %x, sock: %p", g_query_cdn_connect_req, g_query_cdn_socket);
        uv_tcp_init(get_loop(), &request->_socket);
    }

    //if (g_query_cdn_connect_req == NULL)
    {
        uv_connect_t *connect_req = malloc(sizeof(uv_connect_t));

        struct sockaddr_in dest_addr;

        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(request->_port);
        dest_addr.sin_addr.s_addr = request->_ip;

        connect_req->data = request;

        char tmp[100];
        uv_ip4_name(&dest_addr, tmp, 100);


        int ret = uv_tcp_connect(connect_req, &request->_socket, (struct sockaddr *)&dest_addr, connect_cb);
        LOG_DEBUG("start connect %s, ret: %d, conn_req: %p, sock: %p", tmp, ret, g_query_cdn_connect_req, g_query_cdn_socket);
    }
        return 0;
}