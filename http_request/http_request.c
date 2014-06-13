#include "http_request.h"

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

    pair->key = malloc(strlen(key) + 1);
    strcpy(pair->key, key);
    pair->value = malloc(strlen(value) + 1);
    strcpy(pair->value, value);

    struct KV_PAIR *find_res = RB_FIND(KV_MAP_RB, &request->heads, pair);

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
            RB_REMOVE(KV_MAP_RB, &request->heads, find_res);
            free(find_res->key);
            free(find_res->value);
            free(find_res);
        }
    }
    else
    {
        if (value != NULL)
            RB_INSERT(KV_MAP_RB, &request->heads, pair);
    }
    return 0;
}

int start_request(HTTP_REQUEST *request);

const char *get_reponse_head(HTTP_REQUEST *request, char *key)
{
    if(request->reponse.result == -1)
        return NULL;

    struct KV_PAIR pair;
    pair.key = key;
    pair.value = NULL;

    struct KV_PAIR *find_res = RB_FIND(KV_MAP_RB, &request->reponse.heads, &pair);

    if (find_res)
        return find_res->value;

    return NULL;
}



//////////////////////////////////////////////////////////////////////////////////