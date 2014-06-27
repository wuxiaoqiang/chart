#include "stdio.h"

#define LOG_DEBUG(fmt, ...) printf("[%s:%d] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)