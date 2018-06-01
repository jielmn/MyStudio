#=====================================================================
# 说明：本makefile用来编译linux版本的codecwrapper各个库
#---------------------------------------------------------------------

#[指定当前路径为工作路径]
#---------------------------------------------------------------------
WORKSPACE  := $(shell pwd)

COM_DIR    := ../../common/include
INC_DIR    := ../include
SRC_DIR    := ../source
COMM_DIR   := ../../../../10-common/include

MVTOOL_PREFIX     := /opt/montavista/pro/devkit/arm/v5t_le/bin/arm_v5t_le-

#[编译器选择]
#---------------------------------------------------------------------
BDW_CC     := $(MVTOOL_PREFIX)gcc
BDW_AR     := $(MVTOOL_PREFIX)ar
BDW_LD     := $(MVTOOL_PREFIX)ld

#[主编译选项，含宏定义、头文件路径]
#---------------------------------------------------------------------
BDW_CFLAGS := -fno-omit-frame-pointer -fms-extensions -v -D__ARM_D6446__
                  
INC_PATH += $(INC_DIR) $(COMM_DIR)/weibo

BDW_CFLAGS += $(foreach dir,$(INC_PATH),-I$(dir))



#[编译的源文件和生成的目标文件的依赖关系]
#---------------------------------------------------------------------
PRJ_FILES:=  $(WORKSPACE)/../source/wbCheck.cpp \
             $(WORKSPACE)/../source/wbOption.cpp \
             $(WORKSPACE)/../source/wbRequest.cpp \
             $(WORKSPACE)/../source/wbSession.cpp \
             $(WORKSPACE)/../source/wbString.cpp \
             $(WORKSPACE)/../source/wbURL.cpp \
             $(WORKSPACE)/../source/weibo.cpp \
             $(WORKSPACE)/../source/bbiconv.cpp \
             $(WORKSPACE)/../source/hash.c \
             $(WORKSPACE)/../source/oauth.c \
             $(WORKSPACE)/../source/strconv.cpp \
             $(WORKSPACE)/../source/Urlcode.cpp \
             $(WORKSPACE)/../source/xmalloc.c

PRJ_OBJS :=  $(WORKSPACE)/wbCheck.o \
             $(WORKSPACE)/wbOption.o \
             $(WORKSPACE)/wbRequest.o \
             $(WORKSPACE)/wbSession.o \
             $(WORKSPACE)/wbString.o \
             $(WORKSPACE)/wbURL.o \
             $(WORKSPACE)/weibo.o \
             $(WORKSPACE)/bbiconv.o \
             $(WORKSPACE)/hash.o \
             $(WORKSPACE)/oauth.o \
             $(WORKSPACE)/strconv.o \
             $(WORKSPACE)/Urlcode.o \
             $(WORKSPACE)/xmalloc.o


#[生成的模块的名字和路径指定]:
#---------------------------------------------------------------------
DEBUG_LIB_PATH   := ../../../../10-common/lib/debug/linux_arm_d6446
RELEASE_LIB_PATH := ../../../../10-common/lib/release/linux_arm_d6446
LIBNAME  := weibo


all:	debug release

debug:
	cd $(DEBUG_LIB_PATH);    rm -f lib$(LIBNAME)_d.a
	cd $(WORKSPACE);         rm -f *.o; rm -f *.a
	$(BDW_CC) $(BDW_CFLAGS) -g $(PRJ_FILES) -c 
	$(BDW_AR) crus lib$(LIBNAME)_d.a $(PRJ_OBJS)
	mv -f lib$(LIBNAME)_d.a $(DEBUG_LIB_PATH)/lib$(LIBNAME)_d.a
	

release:
	cd $(RELEASE_LIB_PATH);  rm -f lib$(LIBNAME)_r.a
	cd $(WORKSPACE);         rm -f *.o; rm -f *.a
	$(BDW_CC) $(BDW_CFLAGS) -O2 $(PRJ_FILES) -c 
	$(BDW_AR) crus lib$(LIBNAME)_r.a $(PRJ_OBJS)
	mv -f lib$(LIBNAME)_r.a $(RELEASE_LIB_PATH)/lib$(LIBNAME)_r.a

clean:
	cd $(WORKSPACE);         rm -f *.o; rm -f *.a
	cd $(DEBUG_LIB_PATH);    rm -f lib$(LIBNAME)_d.a
	cd $(RELEASE_LIB_PATH);  rm -f lib$(LIBNAME)_r.a   



