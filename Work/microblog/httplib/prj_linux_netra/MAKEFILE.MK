#=====================================================================
# ˵������makefile��������linux�汾��codecwrapper������
#---------------------------------------------------------------------

#[ָ����ǰ·��Ϊ����·��]
#---------------------------------------------------------------------
WORKSPACE  = $(shell pwd)

COM_DIR    = ../../common/include
INC_DIR    = ../include
SRC_DIR    = ../source
COMM_DIR   = ../../../../10-common/include

CODEGEN_PATH_A8   = /opt/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-

#[������ѡ��]
#---------------------------------------------------------------------\
BDW_CC     = $(CODEGEN_PATH_A8)gcc
BDW_AR     = $(CODEGEN_PATH_A8)ar
BDW_LD     = $(CODEGEN_PATH_A8)g++

#[������ѡ����궨�塢ͷ�ļ�·��]
#---------------------------------------------------------------------
BDW_PRE_DEBUG = -fno-omit-frame-pointer
BDW_PRE_RELEASE = -fomit-frame-pointer
BDW_CFLAGS = -fms-extensions -DTI_816X_BUILD -DPLATFORM_EVM_SI -DUTILS_ASSERT_ENABLE -DTI816X_DVR -D_NETRA600_ -D__LINUX__
                  
INC_PATH += $(INC_DIR) $(COMM_DIR)/weibo

BDW_CFLAGS += $(foreach dir,$(INC_PATH),-I$(dir))



#[�����Դ�ļ������ɵ�Ŀ���ļ���������ϵ]
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


#[���ɵ�ģ������ֺ�·��ָ��]:
#---------------------------------------------------------------------
DEBUG_LIB_PATH   = ../../../../10-common/lib/debug/linux_netra_h600
RELEASE_LIB_PATH = ../../../../10-common/lib/release/linux_netra_h600
LIBNAME  := http


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


