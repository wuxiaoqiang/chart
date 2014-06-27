#include "http_request.h"

#include "../log/log.h"
#include "../test/async_frame.h"

int kv_map_cmp(struct KV_PAIR *a, struct KV_PAIR *b)
{
    return strcmp(a->key, b->key);
}

RB_GENERATE(KV_MAP_RB, KV_PAIR, node, kv_map_cmp);



int set_request_headline(HttpRequest *request, char *headline)
{
    strcpy(request->_headline, headline);
    return 0;
}

int set_request_head(HttpRequest *request, const char *key, const char *value)
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

const char *get_reponse_head(HttpResponse *response, const char *key)
{
    if(response->result == -1)
        return NULL;

    struct KV_PAIR pair;
    pair.key = (char *)key;
    pair.value = NULL;

    struct KV_PAIR *find_res = RB_FIND(KV_MAP_RB, &response->headers, &pair);

    if (find_res)
        return find_res->value;

    return NULL;
}

int build_request_buff(HttpRequest *request, char **buf)
{
    ;
}
