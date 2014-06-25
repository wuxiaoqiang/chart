#include "stdio.h"

#define LOG_DEBUG(fmt, ...) printf("[%s] " fmt "\n", __FUNCTION__, ##__VA_ARGS__)