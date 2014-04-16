
#include "define.h"


typedef struct _tag_http_resource
{
	char url[MAX_URL_LEN];
	char ref_url[MAX_URL_LEN];
	char redirect_url[MAX_URL_LEN];
	BOOL reuse_redirect_url;
	int32 session_num;
}HTTP_RESOURCE;

typedef enum {IDLE, CONNECTING, CONNECTED, FAILURE}SESSION_STATE;
typedef struct _tag_session
{
	HTTP_RESOURCE *res;
	SESSION_STATE state;


}SESSION;

HTTP_RESOURCE *create_http_resource(const char url[], const char ref_url[]);

void destory_http_resource(HTTP_RESOURCE *res);



SESSION *http_create_session(HTTP_RESOURCE *res);

int32 session_start_connect(SESSION *session);

int32 session_requect_data(SESSION *session, int64 pos, int64 len);

int32 close_session(SESSION *session);



int32 session_connect_cb(SESSION *session);

int32 session_handle_response(SESSION *session);

int32 session_handle_recv_data(SESSION *session);

