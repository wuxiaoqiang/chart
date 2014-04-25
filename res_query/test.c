#include "uv.h"
#include <stdio.h>

#include "cdn_queryer.h"



uv_loop_t *get_loop()
{
    printf("loop: %p\n", uv_default_loop());
    return uv_default_loop();
}

uv_tcp_t *sock = NULL;
uv_connect_t *con_req;

void wait_for_a_while(uv_idle_t* handle, int status)
{
    static int counter = 0;
    counter+=1;

    while (counter <= 4 && 0)
    {
        sock = malloc(sizeof(uv_tcp_t));
        uv_tcp_init(get_loop(), sock);
        con_req = malloc(sizeof(uv_connect_t));
        struct sockaddr_in dest_addr;
        uv_ip4_addr("192.168.1.1", 80, &dest_addr);
        if (counter==4)uv_ip4_addr("192.168.1.5", 6211, &dest_addr);

        char tmp[100];
        uv_ip4_name(&dest_addr, tmp, 100);

        con_req->data = (void*)counter;


        int ret = uv_tcp_connect(con_req, sock, (struct sockaddr *)&dest_addr, query_cdn_connect_cb);
        printf("Now quitting.%d, tmp:%s, ret: %d\n", counter, tmp, ret);

        counter += 2;
    }

    if (counter == 1)
    {
        // uv_idle_stop(handle);
        query_cdn("94A71E30F90D3ADE2D959E28F55B9D76A2236B65", 0, 0);
        counter += 5;
    }

    sleep(2);
    // printf("\nwait_for_a_while tid: %d", gettid());
}
    uv_idle_t idler;

int main()
{
    uv_idle_init(get_loop(), &idler);
    uv_idle_start(&idler, wait_for_a_while);


    printf("Now quitting.\n");
    uv_run(get_loop(), UV_RUN_DEFAULT);

    return 0;
}