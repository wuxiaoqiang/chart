

typedef void (*dnsparse_cb)(uv_getaddrinfo_t *resolver, int status, struct addrinfo *res);



uv_getaddrinfo_t *dns_parse(const char host[], const char port[], dnsparse_cb cb, void *arg);

int dns_cancel(uv_getaddrinfo_t *);
