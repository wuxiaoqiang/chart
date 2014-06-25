LOCAL_PATH  := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE  := download

LOCAL_C_INCLUDES  += \
  $(LOCAL_PATH)/../uv

LOCAL_SRC_FILES += \
  async_frame.c \
  dns.c \
  test.c

LOCAL_CFLAGS  += -MD -O2 -Wno-psabi 

LOCAL_STATIC_LIBRARIES += libuv http_request

#LOCAL_LDLIBS  += -lz -llog


include $(BUILD_EXECUTABLE)
#include $(BUILD_SHARED_LIBRARY)
