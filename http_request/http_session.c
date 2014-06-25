#include "http_session.h"

#include "../log/log.h"
#include "../test/async_frame.h"


// socket


void parse_dns_cb(unsigned ip, unsigned short port, void *user_data);

int parse_dns(char *host, unsigned short port, void *cb, void *user_data);

void parse_dns_cb_(uv_getaddrinfo_t *resolver, int status, struct addrinfo *res);


int connect_by_domain(char *host, unsigned short port, void *cb, void *user_data)
{
    void **ud_t = malloc(sizeof(void *)*2);

    ud_t[0] = cb;
    ud_t[1] = user_data;

    parse_dns(host,port, (void*)parse_dns_cb, (void*)ud_t);
}

void parse_dns_cb(unsigned ip, unsigned short port, void *user_data)
{
    void **ud_t = user_data;

    void * cb_t = ud_t[0];
    void * user_data_t = ud_t[1];

    connect_by_ip(ip, port, cb_t, (void*)user_data_t);

}

int parse_dns(char *host, unsigned short port, void *cb, void *user_data)
{
    uv_getaddrinfo_t *resolver = malloc(sizeof(uv_getaddrinfo_t));

    void **ud_t = malloc(sizeof(void *)*2);

    ud_t[0] = cb;
    ud_t[1] = user_data;

    resolver->data = (void*)ud_t;

    int r = uv_getaddrinfo(get_loop(), resolver, parse_dns_cb_, host, "80", NULL);
    return r;
}


void parse_dns_cb_(uv_getaddrinfo_t *resolver, int status, struct addrinfo *res)
{
        LOG_DEBUG("status: %d", status);

        void **ud_t = resolver->data;

    void * cb_t = ud_t[0];
    void * user_data_t = ud_t[1];

    free(resolver);


    if (status <0) 
    {
        LOG_DEBUG("error stop strerror: %s, err_name: %s", uv_strerror(status), uv_err_name(status));
        //cb
    }
    else
    {
        int ip = ((struct sockaddr_in*) res->ai_addr)->sin_addr.s_addr;
        unsigned short port = ((struct sockaddr_in*) res->ai_addr)->sin_port;

        uv_freeaddrinfo(res);

        cb_t(ip, port, user_data_t);

    }
}


int connect_by_ip(unsigned ip, unsigned short port, void *cb, void *user_data)
{
    uv_connect_t *req = malloc(sizeof(uv_connect_t));

        struct sockaddr_in dest_addr;

        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(port);
        dest_addr.sin_addr.s_addr = ip;

void *ud_t = malloc(sizeof(void *)*2);

    ud_t[0] = cb;
    ud_t[1] = user_data;

    req->data = ud_t;

        int ret = uv_tcp_connect(req, g_query_cdn_socket, (struct sockaddr *)&dest_addr, connect_cb_);
}

 void connect_cb_(uv_connect_t* req, int status)
 {
       void *ud_t = resolver->data;

    void * cb_t = ud_t[0];
    void * user_data_t = ud_t[1];

        cb_t(req->uv_stream_t, status, user_data_t);
 }



// inner used static fucntion




//  interface

int http_session_create( HttpSessionSettings* settings, HttpSession** session )
{
    HttpSession *session_t = malloc(sizeof(HttpSession));
    memset(session_t, 0, sizeof(HttpSession));

    *session = session_t;

    return 0;
}

int http_session_connect( HttpSession* handle, const char* host, unsigned short port )
{
    ;
}

int http_session_request( HttpSession* handle, char* request_buf, _u32 request_len );

int http_session_close( HttpSession* handle );

int http_session_destroy( HttpSession* handle );




/*


void send_cb(uv_write_t* req, int status);

void query_cdn_alloc_buf(uv_handle_t* handle,
                            size_t suggested_size,
                            uv_buf_t* buf);



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
    // recv_buf[recv_len] = '\0';

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

void alloc_buf_cb(uv_handle_t* handle,
                            size_t suggested_size,
                            uv_buf_t* buf)
{
    LOG_DEBUG("suggested_size: %d", suggested_size);
    char *recv_buf = malloc(100);
    *buf = uv_buf_init(recv_buf, 100);
}

void send_cb(uv_write_t* req, int status)
{
    LOG_DEBUG("%d status: %d", (int)(req->data), status);

    uv_stream_t *handle = req->handle;
    free(req);

    if (status <0) 
    {
        LOG_DEBUG("error stop strerror: %s, err_name: %s", uv_strerror(status), uv_err_name(status));

        query_cdn_close();
        return;
    }

    uv_read_start(handle, alloc_buf_cb, recv_cb);
}


void close_cb(uv_shutdown_t* req, int status)
{
 LOG_DEBUG("status: %d", status);

    if (status <0)
    {
        LOG_DEBUG("error stop strerror: %s, err_name: %s", uv_strerror(status), uv_err_name(status));
    }

    free(req);

}


static int get_ip(HTTP_SESSION *session)
{
    uv_getaddrinfo_t *dns_req = malloc(sizeof(uv_getaddrinfo_t));

    dns_req->data = session;

    int r = uv_getaddrinfo(get_loop(), dns_req, get_ip_cb, request->_host, "80", NULL);
    return r;
}


int start_connect(HTTP_REQUEST *request)
{
    LOG_DEBUG();
    //if (g_query_cdn_socket == NULL)
    {
        // LOG_DEBUG("conn_req: %x, sock: %p", g_query_cdn_connect_req, g_query_cdn_socket);
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
        // LOG_DEBUG("start connect %s, ret: %d, conn_req: %p, sock: %p", tmp, ret, g_query_cdn_connect_req, g_query_cdn_socket);
    }
        return 0;
}*/