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

// #define LOG_DEBUG(format, ...) log_ex("[debug][" LOG_MODULE "][%d][%s:%d] " format "\n", gettid(),__FUNCTION__,__LINE__,##__VA_ARGS__);
#define LOG_DEBUG(format, ...) log_ex(LEVEL_DEBUG, LOG_MODULE, __FUNCTION__, __LINE__, format "\n", ##__VA_ARGS__);

#define LOG_ERROR(format, ...) log_ex(LEVEL_ERROR, LOG_MODULE, __FUNCTION__, __LINE__, format "\n", ##__VA_ARGS__);

#endif