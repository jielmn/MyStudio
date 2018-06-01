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
BDW_CFLAGS := -fno-omit-frame-pointer -fms-extensions -v -D__ARM_D6446__ -DHTTP_ONLY
                  
INC_PATH += $(INC_DIR) $(COMM_DIR)/weibo

BDW_CFLAGS += $(foreach dir,$(INC_PATH),-I$(dir))



#[编译的源文件和生成的目标文件的依赖关系]
#---------------------------------------------------------------------
PRJ_FILES:=  $(WORKSPACE)/../source/base64.c \
             $(WORKSPACE)/../source/connect.c \
             $(WORKSPACE)/../source/content_encoding.c \
             $(WORKSPACE)/../source/cookie.c \
             $(WORKSPACE)/../source/curl_addrinfo.c \
             $(WORKSPACE)/../source/curl_fnmatch.c \
             $(WORKSPACE)/../source/curl_gethostname.c \
             $(WORKSPACE)/../source/curl_memrchr.c \
             $(WORKSPACE)/../source/curl_rand.c \
             $(WORKSPACE)/../source/curl_threads.c \
             $(WORKSPACE)/../source/easy.c \
             $(WORKSPACE)/../source/escape.c \
             $(WORKSPACE)/../source/fileinfo.c \
             $(WORKSPACE)/../source/formdata.c \
             $(WORKSPACE)/../source/getenv.c \
             $(WORKSPACE)/../source/getinfo.c \
             $(WORKSPACE)/../source/hash.c \
             $(WORKSPACE)/../source/hmac.c \
             $(WORKSPACE)/../source/hostares.c \
             $(WORKSPACE)/../source/hostasyn.c \
             $(WORKSPACE)/../source/hostip.c \
             $(WORKSPACE)/../source/hostip4.c \
             $(WORKSPACE)/../source/hostip6.c \
             $(WORKSPACE)/../source/hostsyn.c \
             $(WORKSPACE)/../source/hostthre.c \
             $(WORKSPACE)/../source/http.c \
             $(WORKSPACE)/../source/http_chunks.c \
             $(WORKSPACE)/../source/http_digest.c \
             $(WORKSPACE)/../source/http_negotiate.c \
             $(WORKSPACE)/../source/if2ip.c \
             $(WORKSPACE)/../source/inet_ntop.c \
             $(WORKSPACE)/../source/inet_pton.c \
             $(WORKSPACE)/../source/llist.c \
             $(WORKSPACE)/../source/md5.c \
             $(WORKSPACE)/../source/memdebug.c \
             $(WORKSPACE)/../source/mprintf.c \
             $(WORKSPACE)/../source/multi.c \
             $(WORKSPACE)/../source/netrc.c \
             $(WORKSPACE)/../source/nonblock.c \
             $(WORKSPACE)/../source/parsedate.c \
             $(WORKSPACE)/../source/pingpong.c \
             $(WORKSPACE)/../source/progress.c \
             $(WORKSPACE)/../source/rawstr.c \
             $(WORKSPACE)/../source/select.c \
             $(WORKSPACE)/../source/sendf.c \
             $(WORKSPACE)/../source/share.c \
             $(WORKSPACE)/../source/slist.c \
             $(WORKSPACE)/../source/socks.c \
             $(WORKSPACE)/../source/speedcheck.c \
             $(WORKSPACE)/../source/splay.c \
             $(WORKSPACE)/../source/sslgen.c \
             $(WORKSPACE)/../source/ssluse.c \
             $(WORKSPACE)/../source/strdup.c \
             $(WORKSPACE)/../source/strequal.c \
             $(WORKSPACE)/../source/strerror.c \
             $(WORKSPACE)/../source/strtok.c \
             $(WORKSPACE)/../source/strtoofft.c \
             $(WORKSPACE)/../source/timeval.c \
             $(WORKSPACE)/../source/transfer.c \
             $(WORKSPACE)/../source/url.c \
             $(WORKSPACE)/../source/version.c \
             $(WORKSPACE)/../source/warnless.c \
             $(WORKSPACE)/../source/wildcard.c 


PRJ_OBJS :=  $(WORKSPACE)/base64.o \
             $(WORKSPACE)/connect.o \
             $(WORKSPACE)/content_encoding.o \
             $(WORKSPACE)/cookie.o \
             $(WORKSPACE)/curl_addrinfo.o \
             $(WORKSPACE)/curl_fnmatch.o \
             $(WORKSPACE)/curl_gethostname.o \
             $(WORKSPACE)/curl_memrchr.o \
             $(WORKSPACE)/curl_rand.o \
             $(WORKSPACE)/curl_threads.o \
             $(WORKSPACE)/easy.o \
             $(WORKSPACE)/escape.o \
             $(WORKSPACE)/fileinfo.o \
             $(WORKSPACE)/formdata.o \
             $(WORKSPACE)/getenv.o \
             $(WORKSPACE)/getinfo.o \
             $(WORKSPACE)/hash.o \
             $(WORKSPACE)/hmac.o \
             $(WORKSPACE)/hostares.o \
             $(WORKSPACE)/hostasyn.o \
             $(WORKSPACE)/hostip.o \
             $(WORKSPACE)/hostip4.o \
             $(WORKSPACE)/hostip6.o \
             $(WORKSPACE)/hostsyn.o \
             $(WORKSPACE)/hostthre.o \
             $(WORKSPACE)/http.o \
             $(WORKSPACE)/http_chunks.o \
             $(WORKSPACE)/http_digest.o \
             $(WORKSPACE)/http_negotiate.o \
             $(WORKSPACE)/if2ip.o \
             $(WORKSPACE)/inet_ntop.o \
             $(WORKSPACE)/inet_pton.o \
             $(WORKSPACE)/llist.o \
             $(WORKSPACE)/md5.o \
             $(WORKSPACE)/memdebug.o \
             $(WORKSPACE)/mprintf.o \
             $(WORKSPACE)/multi.o \
             $(WORKSPACE)/netrc.o \
             $(WORKSPACE)/nonblock.o \
             $(WORKSPACE)/parsedate.o \
             $(WORKSPACE)/pingpong.o \
             $(WORKSPACE)/progress.o \
             $(WORKSPACE)/rawstr.o \
             $(WORKSPACE)/select.o \
             $(WORKSPACE)/sendf.o \
             $(WORKSPACE)/share.o \
             $(WORKSPACE)/slist.o \
             $(WORKSPACE)/socks.o \
             $(WORKSPACE)/speedcheck.o \
             $(WORKSPACE)/splay.o \
             $(WORKSPACE)/sslgen.o \
             $(WORKSPACE)/ssluse.o \
             $(WORKSPACE)/strdup.o \
             $(WORKSPACE)/strequal.o \
             $(WORKSPACE)/strerror.o \
             $(WORKSPACE)/strtok.o \
             $(WORKSPACE)/strtoofft.o \
             $(WORKSPACE)/timeval.o \
             $(WORKSPACE)/transfer.o \
             $(WORKSPACE)/url.o \
             $(WORKSPACE)/version.o \
             $(WORKSPACE)/warnless.o \
             $(WORKSPACE)/wildcard.o 


#[生成的模块的名字和路径指定]:
#---------------------------------------------------------------------
DEBUG_LIB_PATH   := ../../../../10-common/lib/debug/linux_arm_d6446
RELEASE_LIB_PATH := ../../../../10-common/lib/release/linux_arm_d6446
LIBNAME  := curl


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



