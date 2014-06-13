LOCAL_PATH  := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE  := tests

LOCAL_C_INCLUDES  += \
  $(LOCAL_PATH)/../uv

LOCAL_SRC_FILES += \
  test_uv_map.c \
  tests.c


LOCAL_CFLAGS  += -MD -O2 -Wno-psabi 

LOCAL_STATIC_LIBRARIES += libuv

#LOCAL_LDLIBS  += -lz -llog


include $(BUILD_EXECUTABLE)
#include $(BUILD_SHARED_LIBRARY)
