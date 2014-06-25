#include "http_request.h"
#include "http_parser.h"

typedef struct tag_http_session
{
    HttpResponse reponse;

    struct http_parser parser;


    uv_tcp_t _socket;

    char *kv_tmp_buf;
}HttpSession;

typedef struct tagHttpSessionHandle HttpSessionHandle;

typedef int (*http_session_connect_cb) (HttpSession*);
typedef int (*http_session_recv_head_cb) (HttpSession*, HttpResponse* resp );
typedef int (*http_session_recv_body_cb) (HttpSession*, char* buff, int len);
typedef int (*http_session_network_error_cb) (HttpSession*, int errCode);
typedef int (*http_session_close_complete_cb) (HttpSession*);

typedef struct tagHttpSessionHandleSettings {
    void *user_data;
    http_session_connect_cb on_connect;
    http_session_recv_head_cb on_head;
    http_session_recv_body_cb on_body;
    http_session_network_error_cb on_error;
    http_session_close_complete_cb on_close;
}HttpSessionSettings;

int http_session_create( HttpSessionSettings* settings, HttpSession** handle );

int http_session_connect( HttpSession* handle, const char* host, unsigned short port );

int http_session_request( HttpSession* handle, char* request_buf, int request_len );

int http_session_close( HttpSession* handle );

int http_session_destroy( HttpSession* handle );

void* get_http_session_user_ptr( HttpSession* handle ); 