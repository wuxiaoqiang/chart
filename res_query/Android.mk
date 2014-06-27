LOCAL_PATH  := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE  := res_query

LOCAL_C_INCLUDES  += \
  $(LOCAL_PATH)/../uv

LOCAL_SRC_FILES += \
  cdn_queryer.c \
  test.c
  

LOCAL_CFLAGS  += -MD -O2

LOCAL_STATIC_LIBRARIES += libuv

#LOCAL_LDLIBS  += -lz -llog


include $(BUILD_EXECUTABLE)
#include $(BUILD_SHARED_LIBRARY)
