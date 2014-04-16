#include "async_frame.h"

#include "define.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uv_getaddrinfo_t resolver;

void on_resolved(uv_getaddrinfo_t *resolver, int status, struct addrinfo *res);

typedef void (*dnsparse_cb)(void *);

int dnsparse(const char host[], const char port[])
{
	struct addrinfo hints;
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = 0;


    int r = uv_getaddrinfo(get_loop(), &resolver, on_resolved, host, "80", NULL);

    printf("getaddrinfo call error %d\n", r);
    if (r) {
        return 1;
    }

    return SUCCESS;

   // return uv_run(loop, UV_RUN_DEFAULT);
}

void on_resolved(uv_getaddrinfo_t *r, int status, struct addrinfo *res)
{
    printf("getaddrinfo callback error %d\n", status);

    printf("%x %x\n", r, &resolver);

    if (status == -1) {
        return;
    }
	printf("%s \n", (char *)r->data);
    char addr[100] = {'\0'};
    uv_ip4_name((struct sockaddr_in*) res->ai_addr, addr, 100);
    printf("ip: %s\n", addr);
    int a = ((struct sockaddr_in*) res->ai_addr)->sin_addr.s_addr;
    printf("%d\n", a);

    // uv_connect_t *connect_req = (uv_connect_t*) malloc(sizeof(uv_connect_t));
    // uv_tcp_t *socket = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    // uv_tcp_init(get_loop(), socket);

    // connect_req->data = (void*) socket;
    //uv_tcp_connect(connect_req, socket, *(struct sockaddr_in*) res->ai_addr, on_connect);

    uv_freeaddrinfo(res);
}