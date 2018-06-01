LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := tinyxml

TINYXML_SRC_DIR		:= ../source
TINYXML_INC_DIR		:= ../include
COMM_DIR   		:= ../../../../10-common/include
ANDROID_COM		:= ../../../../a0-truetouch/Android_Phone/TrueTouch_NEW/jni


TINYXML_SRC_CFILES	:= $(wildcard $(LOCAL_PATH)/$(TINYXML_SRC_DIR)/*.c)
TINYXML_SRC_CPPFILES	:= $(wildcard $(LOCAL_PATH)/$(TINYXML_SRC_DIR)/*.cpp)
TINYXML_SRC_CCFILES	:= $(wildcard $(LOCAL_PATH)/$(TINYXML_SRC_DIR)/*.cc)
		
LOCAL_SRC_FILES 	:= $(subst $(LOCAL_PATH)/,,$(TINYXML_SRC_CFILES))\
			$(subst $(LOCAL_PATH)/,,$(TINYXML_SRC_CPPFILES))\
			$(subst $(LOCAL_PATH)/,,$(TINYXML_SRC_CCFILES))
		
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../common
		

LOCAL_CFLAGS += -D_ANDROID_ -DREFUSE_ESCAPE
LOCAL_CFLAGS += -fpic
LOCAL_LDLIBS := -llog


LOCAL_PRELINK_MODULE:= false
LOCAL_LDLIBS := -lz -ldl -llog 
#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
