LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

subdirs += $(LOCAL_PATH)/../weibo.mk

include $(subdirs)
