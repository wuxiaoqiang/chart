#include "http_session.h"

// typedef struct tagHttpSessionHandleSettings {
//     void *user_data;
//     http_session_connect_cb on_connect;
//     http_session_recv_head_cb on_head;
//     http_session_recv_body_cb on_body;
//     http_session_network_error_cb on_error;
//     http_session_close_complete_cb on_close;
// }HttpSessionSettings;

int test_http_request()
{
    HttpSessionSettings cbs;
    HttpSession * session;
    http_session_create( &cbs, &session );

http_session_connect( session, "www.baidu.com", 80 );

http_session_request( session, "GET / HTTP/1.0", sizeof("GET / HTTP/1.0") );

http_session_close( session);

http_session_destroy( session);
    return 0;
}