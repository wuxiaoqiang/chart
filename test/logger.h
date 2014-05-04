#ifndef _LOGGER_H_
#define _LOGGER_H_

void log_init(char* cfg_path);
void log_ex(int level, int module, const char* func, unsigned int line, const char* fmt, ...);
void log_uninit();

#ifndef LOG_MODULE
#error Compile error, must define LOG_MODULE
#endif

#define LOG_DEBUG(format, ...) log_ex(LOG_LEVEL_DEBUG, LOG_MODULE, __FUNCTION__, __LINE__, format "\n", ##__VA_ARGS__)

#define LOG_ERROR(format, ...) log_ex(LOG_LEVEL_ERROR, LOG_MODULE, __FUNCTION__, __LINE__, format "\n", ##__VA_ARGS__)

#include <android/log.h>

#define STD_LOG_DEBUG(format, ...) __android_log_print(ANDROID_LOG_DEBUG  , "downloadengine", "%d[%s:%d] " format, gettid(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define STD_LOG_INFO(format, ...) __android_log_print(ANDROID_LOG_INFO  , "downloadengine", "[%s:%d] " format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define STD_LOG_WARN(format, ...) __android_log_print(ANDROID_LOG_WARN  , "downloadengine","[%s:%d] " format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define STD_LOG_ERROR(format, ...) __android_log_print(ANDROID_LOG_ERROR  , "downloadengine", "[%s:%d] " format, __FUNCTION__, __LINE__, ##__VA_ARGS__)


#define LOGID_MAP(XX)                                 \
  XX(ASYNC_FRAME, "async_frame")                      \
  XX(MOVIE_MANAGER, "movie_manager")                  \
  XX(DISPATCHER, "dispatcher")                        \
  XX(CACHE, "cache")                                  \
  XX(TASK_MANAGER, "task_manager")                    \


#define LOG_LEVEL_MAP(XX)                             \
  XX(LEVEL_DEBUG, "debug")                            \
  XX(LEVEL_ERROR, "error")                            \

typedef enum {
  // UV_UNKNdOWN_HANDLE = 0,
#define XX(uc, lc) LOG_##uc,
  LOGID_MAP(XX)
#undef XX
  LOG_MODULE_COUNT
}log_ids;

typedef enum {
#define XX(uc, lc) LOG_##uc,
  LOG_LEVEL_MAP(XX)
#undef XX
  LOG_LEVEL_OFF
}log_levles;

#endif