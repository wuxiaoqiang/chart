#include "wwl_socket.h"
#include "wwl_socket_cb_handle.h"

#include "uv.h"

typedef void (*wwl_accept_cb)(int sockfd, int acceptfd, struct sockaddr_in *addr, int result, void *user_data);
typedef void (*wwl_connect_cb)(int sockfd, int result, void *user_data);

typedef void (*www_send_cb)(int sockfd, const void *buf, size_t send_len, int result, void *user_data);
typedef void (*www_recv_cb)(int sockfd, const void *buf, size_t recv_len, int result, void *user_data);



uv_loop_t *get_loop() {return 0;}




int wwl_socket(int domain, int type, int protocol)
{
    uv_tcp_t *fd = malloc(sizeof(uv_tcp_t));
    uv_tcp_init(get_loop(), fd);
    return (int)fd;
}


// int wwl_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int wwl_bind(int sockfd, const struct sockaddr_in *addr)
{
    return uv_tcp_bind((uv_tcp_t *)sockfd, (struct sockaddr*) addr, 0);
}

int wwl_listen(int sockfd, int backlog)
{
    uv_stream_t * stream = malloc(sizeof(uv_stream_t));
    uv_listen(stream, backlog, uv_connection_cb cb);
}


// int wwl_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen, );
int wwl_accept(int sockfd, struct sockaddr_in *addr, wwl_accept_cb cb, void *user_data);


// int wwl_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int wwl_connect(int sockfd, const struct sockaddr_in *addr, wwl_connect_cb cb, void *user_data)
{
    uv_connect_t *connect = malloc(sizeof(uv_connect_t) + sizeof(wwl_connect_cb) + sizeof(user_data));

    uv_tcp_connect(connect, (uv_tcp_t *)sockfd, (struct sockaddr *)addr, wwl_on_connect);
}


// ssize_t wwl_send(int sockfd, const void *buf, size_t len, int flags);
ssize_t wwl_send(int sockfd, const void *buf, size_t len, wwl_send_cb cb, void *user_data);

// ssize_t wwl_recv(int sockfd, void *buf, size_t len, int flags);
ssize_t wwl_recv(int sockfd, void *buf, size_t len, wwl_recv_cb cb, void *user_data);


// size_t wwl_sendto(int sockfd, const void *buf, size_t len, int flags,
                      // const struct sockaddr *dest_addr, socklen_t addrlen);
size_t wwl_sendto(int sockfd, const void *buf, size_t len,
                      const struct sockaddr_in *dest_addr);

// ssize_t wwl_recvfrom(int sockfd, void *buf, size_t len, int flags,
                      // struct sockaddr *src_addr, socklen_t *addrlen);
ssize_t wwl_recvfrom(int sockfd, void *buf, size_t len, struct sockaddr_in *src_addr);


