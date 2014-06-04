#ifndef WWL_SOCKET_H
#define WWL_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


typedef void (* wwl_accept_cb)(int sockfd, int acceptfd, struct sockaddr_in *addr, int result, void *user_data);
typedef void (* wwl_connect_cb)(int sockfd, int result, void *user_data);

typedef void (*wwl_send_cb)(int sockfd, const void *buf, size_t send_len, int result, void *user_data);
typedef void (*wwl_recv_cb)(int sockfd, const void *buf, size_t recv_len, int result, void *user_data);


int wwl_socket(int domain, int type, int protocol);


// int wwl_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int wwl_bind(int sockfd, const struct sockaddr_in *addr);

int wwl_listen(int sockfd, int backlog);


// int wwl_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen, );
int wwl_accept(int sockfd, struct sockaddr_in *addr, wwl_accept_cb cb, void *user_data);


// int wwl_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int wwl_connect(int sockfd, const struct sockaddr_in *addr, wwl_connect_cb cb, void *user_data);


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


#endif