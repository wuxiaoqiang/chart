#ifndef _LOGGER_H_
#define _LOGGER_H_
// #include <stdio.h>

void log_init(char* cfg_path);
void log_ex(int level, const char* module, const char* func, unsigned int line, const char* fmt, ...);
// void log_ex(const char* fmt, ...);
void log_uninit();

// #define log_ex printf
// #define LOG_DEBUG(format, ...) printf("[%d:%s:%d] " format "\n", gettid(),__FUNCTION__, __LINE__,##__VA_ARGS__)
// #define LOG_ERROR(format, ...) printf("[%d:%s:%d] " format "\n", gettid(),__FUNCTION__, __LINE__,##__VA_ARGS__)

#ifndef LOG_MODULE
#error Compile error, must define LOG_MODULE
#endif

#define LEVEL_DEBUG 0
#define LEVEL_ERROR 1

char* ppp[];

// #define LOG_DEBUG(format, ...) log_ex("[debug][" LOG_MODULE "][%d][%s:%d] " format "\n", gettid(),__FUNCTION__,__LINE__,##__VA_ARGS__);
#define LOG_DEBUG(format, ...) log_ex(LEVEL_DEBUG, ppp[LOG_MODULE], __FUNCTION__, __LINE__, format "--%d\n", ##__VA_ARGS__, ASYNC_FRAME);

#define LOG_ERROR(format, ...) log_ex(LEVEL_ERROR, ppp[LOG_MODULE], __FUNCTION__, __LINE__, format "\n", ##__VA_ARGS__);

#include <android/log.h>

#define STD_LOG_DEBUG(format, ...) __android_log_print(ANDROID_LOG_DEBUG  , "downloadengine", "%d[%s:%d] " format, gettid(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define STD_LOG_INFO(format, ...) __android_log_print(ANDROID_LOG_INFO  , "downloadengine", "[%s:%d] " format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define STD_LOG_WARN(format, ...) __android_log_print(ANDROID_LOG_WARN  , "downloadengine","[%s:%d] " format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define STD_LOG_ERROR(format, ...) __android_log_print(ANDROID_LOG_ERROR  , "downloadengine", "[%s:%d] " format, __FUNCTION__, __LINE__, ##__VA_ARGS__)



/* Expand this list if necessary. */
#define UV_ERRNO_MAP_(XX)                                                      \
  XX(ASYNC_FRAME, "async_frame")                                         \
  XX(EACCEdS, "permission denied")                                             \
  XX(EADDRINUdSE, "address already in use")                                    \


typedef enum {
  UV_UNKNdOWN_HANDLE = 0,
#define XX(uc, lc) uc,
  UV_ERRNO_MAP_(XX)
#undef XX
  UV_HAdNDLE_TYPE_MAX
} xw1;


#endif