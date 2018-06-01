LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)  
LOCAL_MODULE:= weibo
WEIBO_SRC_DIR		:= ../source
WEIBO_INC_DIR		:= ../include
COMM_DIR		:= ../../../../10-common/include
ANDROID_COM		:= ../../../../a0-truetouch/Android_Phone/TrueTouch_NEW/jni
LOCAL_C_INCLUDES	:= $(LOCAL_PATH)/$(WEIBO_INC_DIR)\
			$(LOCAL_PATH)/$(COMM_DIR)/weibo\
			$(LOCAL_PATH)/$(ANDROID_COM)/common/iconv

WEIBO_SRC_CFILES	:= $(wildcard $(LOCAL_PATH)/$(WEIBO_SRC_DIR)/*.c)
WEIBO_SRC_CPPFILES	:= $(wildcard $(LOCAL_PATH)/$(WEIBO_SRC_DIR)/*.cpp)
WEIBO_SRC_CCFILES	:= $(wildcard $(LOCAL_PATH)/$(WEIBO_SRC_DIR)/*.cc)
		
LOCAL_SRC_FILES 	:= $(subst $(LOCAL_PATH)/,,$(WEIBO_SRC_CFILES))\
			$(subst $(LOCAL_PATH)/,,$(WEIBO_SRC_CPPFILES))\
			$(subst $(LOCAL_PATH)/,,$(WEIBO_SRC_CCFILES))     
                                                    
LOCAL_CFLAGS := -D_ANDROID_ -DLIBICONV_PLUG

LOCAL_CFLAGS += -fpic
#LOCAL_CFLAGS += -g++
LOCAL_CFLAGS += -fexceptions

ifeq ($(PWLIB_SUPPORT),1)
   LOCAL_C_INCLUDES += $(PWLIBDIR)/include/ptlib/unix $(PWLIBDIR)/include
endif


LOCAL_PRELINK_MODULE:= false
#LOCAL_SHARED_LIBRARIES := http curl iconv
#LOCAL_LDLIBS := -llog
LOCAL_LDLIBS := -lz -ldl -llog 


#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)


