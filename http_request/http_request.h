#include "uv.h"
#include "tree.h"

struct KV_PAIR
{
    char *key;
    char *value;

    RB_ENTRY(KV_PAIR) node;
};

int kv_map_cmp(struct KV_PAIR *a, struct KV_PAIR *b);

RB_HEAD(KV_MAP_RB, KV_PAIR);

RB_PROTOTYPE(KV_MAP_RB, KV_PAIR, node, kv_map_cmp);



// struct tag_http_request;
typedef struct tag_http_request HTTP_REQUEST;


typedef struct tag_http_request_cb
{
    void *user_data;
    void (*connect_cb) (HTTP_REQUEST *, int result, void *user_data);
    void (*recv_header_cb) (HTTP_REQUEST *, int result, void *user_data);
    void (*recv_body_cb) (HTTP_REQUEST *, int result, char *buff, int offset, int len);
}HTTP_REQUEST_CB;

typedef struct tag_http_reponse
{
    int result;
    struct KV_MAP_RB headers;

    char buff[2048];
    int buff_pos;
    char head_buff[2048];
    char body_buff[2048];
}HTTP_REPONSE;


typedef struct tag_http_request
{
    char _host[256];
    unsigned _ip;
    unsigned short _port;

    char _headline[1024];

    struct KV_MAP_RB headers;

    HTTP_REPONSE reponse;

    HTTP_REQUEST_CB cb;

    uv_tcp_t _socket;
}HTTP_REQUEST;

int set_request_info(HTTP_REQUEST *request, char *host, unsigned ip, unsigned short port, char *headline);

int set_cb_info(HTTP_REQUEST *request, HTTP_REQUEST_CB *cb);

int set_request_head(HTTP_REQUEST *request, const char *key, const char *value);

int start_request(HTTP_REQUEST *request, const char *data);

const char *get_reponse_head(HTTP_REQUEST *request, char *key);