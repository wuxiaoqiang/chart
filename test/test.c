

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "async_frame.h"
#include "uv.h"
#include "dns.h"

// #include "test_fs.h"

#define LOG_MODULE LOG_ASYNC_FRAME
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

static void on_co(uv_connect_t* req, int status)
{
    // STD_LOG_DEBUG(" status:%d", status);
    printf("status:%d\n", status);
    // req->data = (void*)1;
}

void timer_handler(uv_timer_t* handle, int status)
{
    LOG_DEBUG("%llu", counter);
    LOG_ERROR("%llu", counter);
    counter++;

    // if(counter < 20)
    // {
    //     uv_tcp_t* tmp_sock = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    //     uv_connect_t* tmp_cn = (uv_connect_t*)malloc(sizeof(uv_connect_t));
    //     uv_tcp_init(get_loop(), tmp_sock);
    //     struct sockaddr_in tmpdest;
    //     uv_ip4_addr("192.168.1.5", 6211, &tmpdest);
    //     // uv_ip4_addr(g_log.ip, g_log.port, &tmpdest);
    //     uv_tcp_connect(tmp_cn, tmp_sock, (struct sockaddr*)&tmpdest, on_co);
    // }

    if(counter==30)
    {   
        // printf("x\n");
        uv_timer_stop(&time_req);
        log_uninit();
        uv_stop(get_loop());
    }
}

int test()
{
    int fd = open("/sdcard/test.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
    printf("%d\n", fd);
    pwrite(fd, "1234", 4, 0);
    pwrite(fd, "9876ddddddxxxxxxxxxxd\n", 10, 100);
    close(fd);
}

int main()
{
    // return test();
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
