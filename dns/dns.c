
#include "dns.h"



uv_getaddrinfo_t *dnsparse(const char host[], const char port[], dnsparse_cb cb, void *arg)
{
    uv_getaddrinfo_t *req = malloc(sizeof(uv_getaddrinfo_t));

    struct addrinfo hints;
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = 0;


    int r = uv_getaddrinfo(get_loop(), &resolver, on_resolved, host, "80", NULL);
}
