LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE:= curl
CURL_SRC_DIR    := ../source
CURL_INC_DIR    := ../include
COMM_DIR   	:= ../../../../10-common/include
ANDROID_COM	:= ../../../../a0-truetouch/Android_Phone/TrueTouch_NEW/jni
LOCAL_C_INCLUDES:= $(LOCAL_PATH)/$(CURL_INC_DIR)\
		$(LOCAL_PATH)/$(COMM_DIR)/weibo


CURL_SRC_CFILES		:= $(wildcard $(LOCAL_PATH)/$(CURL_SRC_DIR)/*.c)
CURL_SRC_CPPFILES	:= $(wildcard $(LOCAL_PATH)/$(CURL_SRC_DIR)/*.cpp)
CURL_SRC_CCFILES	:= $(wildcard $(LOCAL_PATH)/$(CURL_SRC_DIR)/*.cc)
		
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(CURL_SRC_CFILES))\
		$(subst $(LOCAL_PATH)/,,$(CURL_SRC_CPPFILES))\
		$(subst $(LOCAL_PATH)/,,$(CURL_SRC_CCFILES))
		
LOCAL_CFLAGS += -D__ARM_D6446__ -DHTTP_ONLY -D_ANDROID_
#-DPWLIB_SUPPORT=0                                  
LOCAL_CFLAGS += -fpic
LOCAL_CFLAGS += -fexceptions

ifeq ($(PWLIB_SUPPORT),1)
   LOCAL_C_INCLUDES += $(PWLIBDIR)/include/ptlib/unix $(PWLIBDIR)/include
endif

LOCAL_PRELINK_MODULE:= false
LOCAL_LDLIBS += -lz -llog -ldl

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)