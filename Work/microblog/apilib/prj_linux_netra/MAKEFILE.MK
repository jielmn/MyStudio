#=====================================================================
# 说明：本makefile用来编译linux版本的codecwrapper各个库
#---------------------------------------------------------------------

#[指定当前路径为工作路径]
#---------------------------------------------------------------------
WORKSPACE  = $(shell pwd)

COM_DIR    = ../../common/include
INC_DIR    = ../include
SRC_DIR    = ../source
COMM_DIR   = ../../../../10-common/include

CODEGEN_PATH_A8   = /opt/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-

#[编译器选择]
#---------------------------------------------------------------------\
BDW_CC     = $(CODEGEN_PATH_A8)gcc
BDW_AR     = $(CODEGEN_PATH_A8)ar
BDW_LD     = $(CODEGEN_PATH_A8)g++

#[主编译选项，含宏定义、头文件路径]
#---------------------------------------------------------------------
BDW_PRE_DEBUG = -fno-omit-frame-pointer
BDW_PRE_RELEASE = -fomit-frame-pointer
BDW_CFLAGS = -fms-extensions -DTI_816X_BUILD -DPLATFORM_EVM_SI -DUTILS_ASSERT_ENABLE -DTI816X_DVR -D_NETRA600_
                  
INC_PATH += $(INC_DIR) $(COMM_DIR)/weibo

BDW_CFLAGS += $(foreach dir,$(INC_PATH),-I$(dir))



#[编译的源文件和生成的目标文件的依赖关系]
#---------------------------------------------------------------------
PRJ_FILES:=  $(WORKSPACE)/../source/KedaWeiBoAPI.cpp

PRJ_OBJS :=  $(WORKSPACE)/KedaWeiBoAPI.o


#[生成的模块的名字和路径指定]:
#---------------------------------------------------------------------
DEBUG_LIB_PATH   = ../../../../10-common/lib/debug/linux_netra_h600
RELEASE_LIB_PATH = ../../../../10-common/lib/release/linux_netra_h600
LIBNAME  := api


all:	debug release

debug:
	cd $(DEBUG_LIB_PATH);    rm -f lib$(LIBNAME)_d.a
	cd $(WORKSPACE);         rm -f *.o; rm -f *.a
	$(CODEGEN_PATH_A8)g++ -c -Wall $(BDW_PRE_DEBUG) $(BDW_CFLAGS) -g $(PRJ_FILES)
	$(BDW_AR) crus lib$(LIBNAME)_d.a $(PRJ_OBJS)
	mv -f lib$(LIBNAME)_d.a $(DEBUG_LIB_PATH)/lib$(LIBNAME)_d.a
	

release:
	cd $(RELEASE_LIB_PATH);  rm -f lib$(LIBNAME)_r.a
	cd $(WORKSPACE);         rm -f *.o; rm -f *.a
	$(CODEGEN_PATH_A8)g++ -c -Wall $(BDW_PRE_RELEASE) $(BDW_CFLAGS) -O2 $(PRJ_FILES)
	$(BDW_AR) crus lib$(LIBNAME)_r.a $(PRJ_OBJS)
	mv -f lib$(LIBNAME)_r.a $(RELEASE_LIB_PATH)/lib$(LIBNAME)_r.a

clean:
	cd $(WORKSPACE);         rm -f *.o; rm -f *.a
	cd $(DEBUG_LIB_PATH);    rm -f lib$(LIBNAME)_d.a
	cd $(RELEASE_LIB_PATH);  rm -f lib$(LIBNAME)_r.a   



