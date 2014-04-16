LOCAL_PATH  := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libuv

LOCAL_SRC_FILES := libuv.a

include $(PREBUILT_STATIC_LIBRARY)