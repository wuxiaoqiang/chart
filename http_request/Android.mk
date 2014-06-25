LOCAL_PATH  := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE  := http_request

LOCAL_C_INCLUDES  += \
  $(LOCAL_PATH)/../uv

LOCAL_SRC_FILES += \
  http_request.c \
  http_parser.c \
  http_session.c \
  test_http_request.c \


LOCAL_CFLAGS  += -MD -O2 -Wno-psabi 

LOCAL_STATIC_LIBRARIES += libuv

#LOCAL_LDLIBS  += -lz -llog


#include $(BUILD_EXECUTABLE)
include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)
