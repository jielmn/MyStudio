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
PRJ_FILES:=  $(WORKSPACE)/../source/bbiconv.cpp \
             $(WORKSPACE)/../source/loBytebuffer.cpp \
             $(WORKSPACE)/../source/loFile.cpp \
             $(WORKSPACE)/../source/loHttp+.cpp \
             $(WORKSPACE)/../source/loHttp.cpp \
             $(WORKSPACE)/../source/loHttpAssoc.cpp \
             $(WORKSPACE)/../source/loHttpCurl.cpp \
             $(WORKSPACE)/../source/loHttpStr.cpp \
             $(WORKSPACE)/../source/thpool.c \
			 $(WORKSPACE)/../source/spthread.c \
             $(WORKSPACE)/../source/lostrconv.cpp \
             $(WORKSPACE)/../source/loUrlcode.cpp

PRJ_OBJS :=  $(WORKSPACE)/bbiconv.o \
             $(WORKSPACE)/loBytebuffer.o \
             $(WORKSPACE)/loFile.o \
             $(WORKSPACE)/loHttp+.o \
             $(WORKSPACE)/loHttp.o \
             $(WORKSPACE)/loHttpAssoc.o \
             $(WORKSPACE)/loHttpCurl.o \
             $(WORKSPACE)/loHttpStr.o \
             $(WORKSPACE)/thpool.o \
			 $(WORKSPACE)/spthread.o \
             $(WORKSPACE)/lostrconv.o \
             $(WORKSPACE)/loUrlcode.o


#[生成的模块的名字和路径指定]:
#---------------------------------------------------------------------
DEBUG_LIB_PATH   := ../../../../10-common/lib/debug/linux_arm_d6446
RELEASE_LIB_PATH := ../../../../10-common/lib/release/linux_arm_d6446
LIBNAME  := http


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



