LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

subdirs += $(LOCAL_PATH)/../tinyxml.mk

include $(subdirs)
