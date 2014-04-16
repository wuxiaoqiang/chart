#include "http.h"

#include <stdlib.h>
#include <string.h>

#include "uv.h"
#include "async_frame.h"


HTTP_RESOURCE *create_http_resource(const char url[], const char ref_url[])
{
	HTTP_RESOURCE *res = (HTTP_RESOURCE *)calloc(1, sizeof(HTTP_RESOURCE));

	strcpy(res->url, url);
	strcpy(res->ref_url, ref_url);


	return res;
}

void destory_http_resource(HTTP_RESOURCE *res)
{
	free(res);
}



SESSION *http_create_session(HTTP_RESOURCE *res)
{
	SESSION *session = (SESSION *)calloc(1, sizeof(SESSION));

	session->res = res;
	session->state = IDLE;


	return session;

}

int32 session_start_connect(SESSION *session)
{
	uv_connect_t connect;
	uv_tcp_t socket;
	uv_tcp_init(get_loop(), &socket);


	struct sockaddr_in dest;
	uv_ip4_addr("127.0.0.1", 80, &dest);
	uv_connect_cb on_connect = NULL;
	uv_tcp_connect(&connect, &socket, (struct sockaddr *)&dest, on_connect);

	return SUCCESS;
}

int32 session_requect_data(SESSION *session, int64 pos, int64 len);

int32 close_session(SESSION *session);



int32 session_connect_cb(SESSION *session);

int32 session_handle_response(SESSION *session);

int32 session_handle_recv_data(SESSION *session);