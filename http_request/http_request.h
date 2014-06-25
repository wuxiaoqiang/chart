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



typedef struct tag_http_reponse
{
    int result;
    struct KV_MAP_RB headers;
}HttpResponse;

const char *get_response_head(HttpResponse *response, const char *key);




typedef struct tag_http_request
{
    char _headline[1024];

    struct KV_MAP_RB headers;
}HttpRequest;

int set_request_headline(HttpRequest *request, char *headline);

int set_request_head(HttpRequest *request, const char *key, const char *value);

int build_request_buff(HttpRequest *request, char **buf);