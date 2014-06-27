LOCAL_PATH  := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE  := wwl_socket

LOCAL_C_INCLUDES  += \
  $(LOCAL_PATH)/../uv

LOCAL_SRC_FILES += \
  wwl_socket.c \
  wwl_socket_cb_handle.c \
  test_socket.c

LOCAL_CFLAGS  += -MD -O2 -Wno-psabi

LOCAL_STATIC_LIBRARIES += libuv

#include $(BUILD_EXECUTABLE)
include $(BUILD_STATIC_LIBRARY)
