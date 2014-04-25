#include <stdarg.h>
#include "uv.h"
#include "async_frame.h"
#define LOG_MODULE "logger"
#include "logger.h"

#include <stdio.h>
#define _LOG_DEBUG(format, ...) printf("[%d:%s:%d] " format "\n", gettid(),__FUNCTION__, __LINE__,##__VA_ARGS__)
#define _LOG_ERROR(format, ...) printf("[%d:%s:%d] " format "\n", gettid(),__FUNCTION__, __LINE__,##__VA_ARGS__)

#define LEVEL_BUF_0_ "debug"
#define LEVEL_BUF_1_ "error"

char* ppp[]=
{
  "",
#define XX(uc, lc) lc,
  UV_ERRNO_MAP_(XX)
#undef XX
  "UV_HANDLE_TYPE_MAX"
} ;


static const char* g_level[] = {"debug","error"};


// #define LEVEL_BUF(level) LEVEL_BUF_##level##_	
#define LEVEL_BUF(level) g_level[level]

#define MAX_MODULE_NUM 512

struct log_buf_struct
{
	struct log_buf_struct* next;
	char* buf;
};
enum log_type{LOG_TYPE_LOCAL, LOG_TYPE_NET};
struct log_struct
{
	enum log_type type;
	char ip[64];
	int port;

	char path[1024];
	char name[1024];
	unsigned int max_num;
	unsigned int max_size;
	unsigned int cur_num;
	int	cur_file;

	int default_level;
	int module_num;
	char* modules[MAX_MODULE_NUM];
	int levels[MAX_MODULE_NUM];


	struct log_buf_struct* head;
	struct log_buf_struct* tail;
	uv_mutex_t lock;
	// uint64_t wait_log_num;
	int flushing;
	int notify_close;
	uv_timer_t time_req;

	uv_tcp_t socked;
	uv_connect_t connect;

	uv_pipe_t pipe;
};

static struct log_struct g_log;

static void join_path(const char* path, const char* name, char* file_path)
{
	unsigned int len = strlen(path);
	strncpy(file_path, path, len);
	if(file_path[len-1]!='/')
	{
		file_path[len] = '/';
		len++;
	}
	strncpy(file_path+len, name, strlen(name));
}

static void rename_logs()
{
	char name[1024] = {0};
	join_path(g_log.path, g_log.name, name);
	char old_log_name[1024] = {0};
	char new_log_name[1024] = {0};

	int i;
	for(i=g_log.max_num; i>=0; --i)
	{
		memset(old_log_name, 0, 1024);
	
		if(i>0)
			snprintf(old_log_name, 1024, "%s.%d", name, i);
		else
			snprintf(old_log_name, 1024, "%s", name);

		if(access( old_log_name, F_OK ) != -1 )
		{
			if(i==g_log.max_num)
			{
				remove(old_log_name);
			}
			else
			{
				memset(new_log_name, 0, 1024);
				snprintf(new_log_name, 1024, "%s.%d", name, i+1);
				rename(old_log_name, new_log_name);
			}
		}
	}
}

static void on_log_server_connect(uv_connect_t* req, int status)
{
	_LOG_DEBUG("status:%d", status);
	req->data = (void*)1;
}

static void flush_log();

static void logger_timer_handler(uv_timer_t* handle, int status)
{
	flush_log();
}

void log_init(char* cfg_path)
{
	if(cfg_path && access(cfg_path, F_OK)!=-1)
	{
		char buf[1024] = {0};
		FILE* fp;
		fp = fopen(cfg_path, "r");
		while(fgets(buf, 1023, fp))
		{
			buf[strlen(buf)-1]='\0';

			if(strlen(buf)>0 && buf[0]!='\n' && buf[0]!='#')
			{
				char* key = strtok(buf, "=");
				char* value = strtok(NULL, "=");

				char* section = strtok(key, ".");
				char* detail = strtok(NULL, ".");

				if(strcmp(section, "root")==0)
				{
					if(strcmp(detail, "level")==0)
					{
						if(strcmp(value, "debug")==0)
							g_log.default_level = LEVEL_DEBUG;
						else if(strcmp(value, "error")==0)
							g_log.default_level = LEVEL_ERROR;
						else
							g_log.default_level = 100;
					}
					else if(strcmp(detail, "type")==0)
					{
						if(strcmp(value, "local")==0)
							g_log.type = LOG_TYPE_LOCAL;
						else if(strcmp(value, "net")==0)
							g_log.type = LOG_TYPE_NET;
					}
					else if(strcmp(detail, "path")==0)
					{
						memcpy(g_log.path, value, strlen(value));
					}
					else if(strcmp(detail, "name")==0)
					{
						memcpy(g_log.name, value, strlen(value));
					}
					else if(strcmp(detail, "max_size")==0)
					{
						sscanf(value, "%d", &g_log.max_size);
					}
					else if(strcmp(detail, "max_num")==0)
					{
						sscanf(value, "%d", &g_log.max_num);
					}
					else if(strcmp(detail, "ip")==0)
					{
						memcpy(g_log.ip, value, strlen(value));
					}
					else if(strcmp(detail, "port")==0)
					{
						sscanf(value,"%d", &g_log.port);
					}

				}
				else if(strcmp(section, "module")==0)
				{
					char* tmp_module = (char*)malloc(strlen(detail)+1);
					memcpy(tmp_module, detail, strlen(detail));
					tmp_module[strlen(detail)]='\0';
					if(strcmp(value, "debug")==0)
					{
						g_log.modules[g_log.module_num] = tmp_module;
						g_log.levels[g_log.module_num] = LEVEL_DEBUG;
					}
					else if(strcmp(value, "error")==0)
					{
						g_log.modules[g_log.module_num] = tmp_module;
						g_log.levels[g_log.module_num] = LEVEL_ERROR;
					}
					++g_log.module_num;
				}
			}
			memset(buf, 0, 1024);
		}
		fclose(fp);
	}
	// g_log.type = LOG_TYPE_LOCAL;
	if(g_log.type == LOG_TYPE_LOCAL)
	{	
		// g_log.max_size = 512*1024;
		rename_logs();
		char path[1024] = {0};
		join_path(g_log.path, g_log.name, path);
		g_log.cur_file = open(path, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
	}
	else
	{
		uv_tcp_init(get_loop(), &g_log.socked);

		struct sockaddr_in dest;
		uv_ip4_addr(g_log.ip, g_log.port, &dest);
		// uv_ip4_addr("220.181.111.85", 80, &dest);

		uv_tcp_connect(&g_log.connect, &g_log.socked, (struct sockaddr*)&dest, on_log_server_connect);


		// uv_tcp_t* tmp_sock = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
		// uv_connect_t* tmp_cn = (uv_connect_t*)malloc(sizeof(uv_connect_t));
		// uv_tcp_init(get_loop(), tmp_sock);
		// struct sockaddr_in tmpdest;
		// // uv_ip4_addr("220.181.111.85", 80, &tmpdest);
		// uv_ip4_addr(g_log.ip, g_log.port, &tmpdest);
		// uv_tcp_connect(tmp_cn, tmp_sock, (struct sockaddr*)&tmpdest, on_log_server_connect);

	}

	uv_mutex_init(&g_log.lock);

	uv_pipe_init(get_loop(), &g_log.pipe, 0);

	uv_timer_init(get_loop(), &g_log.time_req);
	uv_timer_start(&g_log.time_req, logger_timer_handler, 200, 1000);
}

void log_uninit_ex()
{
	close(g_log.cur_file);

	int i;
	for(i=0; i<g_log.module_num; ++i)
	{
		free(g_log.modules[i]);
	}
	uv_mutex_destroy(&g_log.lock);
	uv_timer_stop(&g_log.time_req);
	uv_close((uv_handle_t*)&g_log.pipe, NULL);

	_LOG_DEBUG("");
	memset(&g_log, 0, sizeof(struct log_struct));
}

static void file_write_callback(uv_fs_t* req);

static void socked_write_callback(uv_write_t* req, int status);

static void flush_log()
{
	// _LOG_DEBUG("flushing:%d", g_log.flushing);
	if(g_log.flushing)
		return;

    uv_buf_t uv_buf[1024] = {0};
    int index = 0;
	struct log_buf_struct* tmp;
	
	uv_mutex_lock(&g_log.lock);
	struct log_buf_struct* head =g_log.head;
	// req->data = g_log.head;
    while(g_log.head!=NULL && index < 1024)
    {
    	uv_buf[index].base = g_log.head->buf;
    	uv_buf[index].len = strlen(g_log.head->buf);
    	++index;
    	tmp = g_log.head;
    	if(g_log.tail==g_log.head)
    		g_log.tail = NULL;
    	g_log.head = g_log.head->next;
    	// --g_log.wait_log_num;
    }
    if(index)
	    g_log.flushing = 1;
	else
		g_log.flushing = 0;
	uv_mutex_unlock(&g_log.lock);

	// _LOG_DEBUG("%d, %d", index, g_log.notify_close);
	if(index)
	{
		tmp->next = NULL;

		if(g_log.type==LOG_TYPE_LOCAL)
		{
			uv_fs_t* req = (uv_fs_t*)malloc(sizeof(uv_fs_t));
			req->data = head;
		    uv_fs_write(get_loop(), req, g_log.cur_file, uv_buf, index, -1, file_write_callback);
		}
		else if(g_log.type==LOG_TYPE_NET)
		{
			uv_write_t* req = (uv_write_t*)malloc(sizeof(uv_write_t));
			req->data = head;
			uv_write(req, (uv_stream_t*)&g_log.socked, uv_buf, index, socked_write_callback);
		}
	}
	else
	{
		if(g_log.notify_close)
			log_uninit_ex();
	}
}

static void socked_write_callback(uv_write_t* req, int status)
{
    struct log_buf_struct* tmp = (struct log_buf_struct*)req->data;
    struct log_buf_struct* pre = tmp;
    while(tmp!=NULL)
    {
    	free(tmp->buf);
    	pre = tmp;
    	tmp = tmp->next;
    	free(pre);
    }
    free(req);
    g_log.flushing = 0;
}

static void file_write_callback(uv_fs_t* req)
{
    uv_fs_req_cleanup(req);

    struct log_buf_struct* tmp = (struct log_buf_struct*)req->data;
    struct log_buf_struct* pre = tmp;
    while(tmp!=NULL)
    {
    	free(tmp->buf);
    	pre = tmp;
    	tmp = tmp->next;
    	free(pre);
    }
    free(req);

    uint64_t filesize = 0;
    struct stat statbuff;
    if(fstat(g_log.cur_file, &statbuff)>=0)
    {
    	filesize = statbuff.st_size;
    }
    if(filesize > g_log.max_size)
    {
    	close(g_log.cur_file);
    	rename_logs();

 	    char new_log_name[1024] = {0};
    	join_path(g_log.path, g_log.name, new_log_name);
    	g_log.cur_file = open(new_log_name, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
    }
    g_log.flushing = 0;
    // flush_log();
}

static int log_this_module(const char* module, int level)
{
	if(g_log.default_level<=level)
		return 1;

	int i;
	for(i=0; i<g_log.module_num; ++i)
	{
		if(strcmp(g_log.modules[i], module)==0)
		{
			if(g_log.levels[i] <= level)
				return 1;
			else
				return 0;
		}
	}
	return 0;
}
// strdup

void log_ex_2(int level, const char* module, const char* func, unsigned int line, const char* fmt, ...)
{
	if(g_log.notify_close)
		return;
	if(g_log.type==LOG_TYPE_LOCAL && g_log.cur_file<=0)
		return;
	if(g_log.type==LOG_TYPE_NET && g_log.connect.data==NULL)
		return;

	if(!log_this_module(module, level))
		return;

	char str[1024] = {0};

	struct tm local_time;
	struct timeval now;

	gettimeofday(&now, NULL);
	localtime_r(&now.tv_sec, &local_time);

	snprintf(str, 1024, "%04d-%02d-%02d %02d:%02d:%02d:%03ld [%s][%s][%d][%s:%d]",
		local_time.tm_year + 1900, local_time.tm_mon + 1, local_time.tm_mday,
		local_time.tm_hour, local_time.tm_min, local_time.tm_sec, now.tv_usec / 1000,
		LEVEL_BUF(level),module,gettid(),func,line);

	va_list args;
	va_start(args, fmt);
	vsnprintf(str+strlen(str), 1024, fmt, args);
	va_end(args);
	
	uv_mutex_lock(&g_log.lock);

	uv_mutex_unlock(&g_log.lock);
}

void log_ex(int level, const char* module, const char* func, unsigned int line, const char* fmt, ...)
{
	// if(g_log.notify_close)
	// 	return;
	// if(g_log.type==LOG_TYPE_LOCAL && g_log.cur_file<=0)
	// 	return;
	// if(g_log.type==LOG_TYPE_NET && g_log.connect.data==NULL)
	// 	return;

	// if(!log_this_module(module, level))
	// 	return;

	char str[1024] = {0};

	struct tm local_time;
	struct timeval now;

	gettimeofday(&now, NULL);
	localtime_r(&now.tv_sec, &local_time);

	snprintf(str, 1024, "%04d-%02d-%02d %02d:%02d:%02d:%03ld [%s][%s][%d][%s:%d]",
		local_time.tm_year + 1900, local_time.tm_mon + 1, local_time.tm_mday,
		local_time.tm_hour, local_time.tm_min, local_time.tm_sec, now.tv_usec / 1000,
		LEVEL_BUF(level),module,gettid(),func,line);

	va_list args;
	va_start(args, fmt);
	vsnprintf(str+strlen(str), 1024, fmt, args);
	va_end(args);

	printf("%s", str);

	char* dup_buf = strdup(str);
	struct log_buf_struct* new_buf_struct = (struct log_buf_struct*)malloc(sizeof(struct log_buf_struct));
	new_buf_struct->buf = dup_buf;
	new_buf_struct->next = NULL; 
	
	uv_mutex_lock(&g_log.lock);

	if(g_log.head==NULL)
	{
		g_log.head = new_buf_struct;
	}
	else
	{
		g_log.tail->next = new_buf_struct;
	}
	g_log.tail = new_buf_struct;

	uv_mutex_unlock(&g_log.lock);
	return;
}

void log_uninit()
{
	_LOG_DEBUG("");
	g_log.notify_close = 1;
	// close(g_log.cur_file);
	// g_log.cur_file = 0;
}