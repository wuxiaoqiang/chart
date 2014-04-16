#ifndef _GLOBAL_DEFINE

#define _GLOBAL_DEFINE


typedef char               int8;
typedef short              int16;
typedef int                int32;
typedef long long          int64;

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef int32 BOOL;
#define TRUE (int32(1));
#define FALSE (int32(0));


#ifndef NULL
#define NULL 0
#endif

#define SUCCESS 0



#define MAX_URL_LEN 1024

#define MAX_PATH_LEN 1024

#define MAX_HOST_LEN 64



#endif
