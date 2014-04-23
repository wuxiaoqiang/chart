

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "async_frame.h"
#include "uv.h"
#include "dns.h"

// #include "test_fs.h"

#define LOG_MODULE "async_frame"
#include "logger.h"

int64_t counter = 0;


void wait_for_a_while(uv_idle_t* handle, int status)
{
    counter++;

    if (counter == 1)
    {        ;//uv_idle_stop(handle);
        dnsparse("www.baidu.com");
        // start_test_fs(get_loop());
    }
    if(counter == 200)
    {
        uv_idle_stop(handle);
        LOG_DEBUG("uv_idle_stop");
        printf("uv_idle_stop\n");
        log_uninit();
        uv_stop(get_loop());
    }
    if(counter==200)
        printf("200\n");
    sleep(1);
    int i;
    for(i=0; i< counter; ++i)
        LOG_DEBUG("wait_for_a_while %d, %llu, tid: %d", i, counter, gettid());
}

void* test_thread(void * arg)
{
    int i;
    for(i=0; i<10; ++i)
    {
        LOG_DEBUG("%d", i);
    }
    return NULL;
}

uv_timer_t time_req;

void timer_handler(uv_timer_t* handle, int status)
{
    LOG_DEBUG("%llu", counter);
    counter++;
    if(counter==30)
    {   
        uv_timer_stop(&time_req);
        log_uninit();
        uv_stop(get_loop());
    }
}

int main()
{
	// uv_idle_t idler;

 //    uv_idle_init(get_loop(), &idler);
 //    uv_idle_start(&idler, wait_for_a_while);

    uv_timer_init(get_loop(), &time_req);
    uv_timer_start(&time_req, timer_handler, 200, 100);

    // start_test_fs(get_loop());
    log_init("/sdcard/test/prop.txt");

    // int i;
    // for(i=0; i<2; ++i)
    // {    
    //     pthread_t tid;
    //     pthread_create(&tid, NULL, test_thread, NULL);
    // }


    LOG_DEBUG("Now quitting.");
    uv_run(get_loop(), UV_RUN_DEFAULT);

    return 0;
}
