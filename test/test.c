

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "async_frame.h"
#include "uv.h"
#include "dns.h"


int64_t counter = 0;


void wait_for_a_while(uv_idle_t* handle, int status)
{
    counter++;

    if (counter == 1)
    {        ;//uv_idle_stop(handle);
        dnsparse("www.baidu.com");
    }
    sleep(2);
    printf("wait_for_a_while tid: %d\n", gettid());
}

int main()
{
    test_http_request();
	// uv_idle_t idler;

 //    uv_idle_init(get_loop(), &idler);
 //    uv_idle_start(&idler, wait_for_a_while);


    printf("Now quitting.\n");
    uv_run(get_loop(), UV_RUN_DEFAULT);

    return 0;
}
