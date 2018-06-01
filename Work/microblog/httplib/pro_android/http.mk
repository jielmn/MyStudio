LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE:= http
HTTP_SRC_DIR    := ../source
HTTP_INC_DIR    := ../include
COMM_DIR   	:= ../../../../10-common/include
ANDROID_COM	:= ../../../../a0-truetouch/Android_Phone/TrueTouch_NEW/jni
LOCAL_C_INCLUDES     := $(LOCAL_PATH)/$(HTTP_INC_DIR)\
		$(LOCAL_PATH)/$(COMM_DIR)/weibo\
		$(LOCAL_PATH)/../../common\
		$(LOCAL_PATH)/../../http_stack\
		$(LOCAL_PATH)/$(ANDROID_COM)/common/iconv


HTTP_SRC_CFILES		:= $(wildcard $(LOCAL_PATH)/$(HTTP_SRC_DIR)/*.c)
HTTP_SRC_CPPFILES	:= $(wildcard $(LOCAL_PATH)/$(HTTP_SRC_DIR)/*.cpp)
HTTP_SRC_CCFILES	:= $(wildcard $(LOCAL_PATH)/$(HTTP_SRC_DIR)/*.cc)
		
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(HTTP_SRC_CFILES))\
		$(subst $(LOCAL_PATH)/,,$(HTTP_SRC_CPPFILES))\
		$(subst $(LOCAL_PATH)/,,$(HTTP_SRC_CCFILES))\
		../../http_stack/httpstack.cpp \
		../../http_stack/inner.cpp \
		../../http_stack/seli.cpp

#$(warning $(LOCAL_SRC_FILES))
LOCAL_CFLAGS += -D__ARM_D6446__ -D_ANDROID_ -DLIBICONV_PLUG
#-DPWLIB_SUPPORT=0                                  
LOCAL_CFLAGS += -fpic
LOCAL_CFLAGS += -fexceptions

ifeq ($(PWLIB_SUPPORT),1)
   LOCAL_C_INCLUDES += $(PWLIBDIR)/include/ptlib/unix $(PWLIBDIR)/include
endif

LOCAL_PRELINK_MODULE:= false
LOCAL_LDLIBS += -lz -llog -ldl

#LOCAL_SHARED_LIBRARIES := curl iconv

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)