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

static const char* g_level[] = {"debug","error"};


// #define LEVEL_BUF(level) LEVEL_BUF_##level##_	
#define LEVEL_BUF(level) g_level[level]

#define MAX_MODULE_NUM 512

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
	uv_file	cur_file;

	int default_level;
	int module_num;
	char* modules[MAX_MODULE_NUM];
	int levels[MAX_MODULE_NUM];
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

static uv_fs_t* malloc_req()
{
    return malloc(sizeof(uv_fs_t));
}

static free_req(uv_fs_t* req)
{
    free(req);
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

	rename_logs();
	char path[1024] = {0};
	join_path(g_log.path, g_log.name, path);
	g_log.cur_file = open(path, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
	LOG_DEBUG("");
}

static void write_callback(uv_fs_t* req)
{
    uv_fs_req_cleanup(req);
    free_req(req);
}

static void on_new_log()
{
	
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
void log_ex(int level, const char* module, const char* func, unsigned int line, const char* fmt, ...)
{
	if(g_log.cur_file<=0)
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

	// printf("%s", str);
	uv_fs_t* req = malloc_req();
    uv_buf_t uv_buf[1] = {0};
    uv_buf[0].base = str;
    uv_buf[0].len = strlen(str);
    uv_fs_write(get_loop(), req, g_log.cur_file, uv_buf, 1, -1, write_callback);

    uint64_t filesize = 0;
    struct stat statbuff;
    if(fstat(g_log.cur_file, &statbuff)>=0)
    {
    	filesize = statbuff.st_size;
    }
    if(filesize > g_log.max_size)
    {
    	char new_log_tmp_name[1024] = {0};
    	join_path(g_log.path, "tmp.log", new_log_tmp_name);
    	uv_file old_file = g_log.cur_file;
    	g_log.cur_file = open(new_log_tmp_name, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
    	close(g_log.cur_file);
    	rename_logs();
    	char new_log_name[1024] = {0};
    	join_path(g_log.path, g_log.name, new_log_name);
    	rename(new_log_tmp_name, new_log_name);
    }
}

void log_uninit()
{
	LOG_DEBUG("");
	close(g_log.cur_file);
	g_log.cur_file = 0;
}