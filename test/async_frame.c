#include "async_frame.h"

#include <stdlib.h>

uv_loop_t *loop = NULL;

uv_loop_t *get_loop()
{
	if (loop == NULL)
	{
		loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
		uv_loop_init(loop);
	}

	return loop;
}

void init_frame()
{
	;
}
