TARGET_DIR  := linux
####################################### Set up the tools to use
ifeq ($(LINUX_COMPILER),_HHPPC_)
CROSS = ppc_82xx-
endif

ifeq ($(LINUX_COMPILER),_PPC85_)
CROSS = ppc_85xx-
endif

ifeq ($(LINUX_COMPILER),_ARM_)
CROSS = /opt/montavista/pro/devkit/arm/v5t_le/bin/arm_v5t_le-
TARGET_DIR  := linux_arm_d6446
endif

ifeq ($(LINUX_COMPILER),_DAVINCI_)
CROSS = /opt/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-
TARGET_DIR  := linux_netra
DEFINE_FLAG := $(DEFINE_FLAG) -D__HD3__
endif
##########################################################################



API_FOLD  = apilib
HTTP_FOLD = httplib
HTTP_STACK_FOLD = http_stack
PARSER_FOLDER = parsewrapperlib
WEIBO_FOLDER  = weibolib

# 标准makefile文件
# omit .o  包括完整的路径，如 $(BASE)/t1 $(CALL)/call
OBJS            :=  	$(API_FOLD)/source/KedaWeiBoAPI \
							\
			$(HTTP_FOLD)/source/bbiconv \
			$(HTTP_FOLD)/source/loBytebuffer \
			$(HTTP_FOLD)/source/loFile  \
			$(HTTP_FOLD)/source/loHttp  \
			$(HTTP_FOLD)/source/loHttp+ \
			$(HTTP_FOLD)/source/loHttpAssoc \
			$(HTTP_FOLD)/source/loHttpCurl \
			$(HTTP_FOLD)/source/loHttpStr  \
			$(HTTP_FOLD)/source/loUrlcode \
			$(HTTP_FOLD)/source/lostrconv 	\
			$(HTTP_FOLD)/source/spthread  \
			$(HTTP_FOLD)/source/thpool  \
							\
			$(HTTP_STACK_FOLD)/httpstack \
			$(HTTP_STACK_FOLD)/inner \
			$(HTTP_STACK_FOLD)/seli  \
			$(HTTP_STACK_FOLD)/ssl_adapter\
						\
			$(PARSER_FOLDER)/source/Urlcode \
			$(PARSER_FOLDER)/source/splitstr \
			$(PARSER_FOLDER)/source/strconv  \
			$(PARSER_FOLDER)/source/wbParseImpl \
			$(PARSER_FOLDER)/source/wbParseMalloc \
			$(PARSER_FOLDER)/source/wbParseObj  \
			$(PARSER_FOLDER)/source/wbParseWrapper \
			$(PARSER_FOLDER)/source/wbParser \
							\
			$(WEIBO_FOLDER)/source/wbCheck \
			$(WEIBO_FOLDER)/source/wbOption  \
			$(WEIBO_FOLDER)/source/wbRequest \
			$(WEIBO_FOLDER)/source/wbSession \
			$(WEIBO_FOLDER)/source/wbString \
			$(WEIBO_FOLDER)/source/wbURL \
			$(WEIBO_FOLDER)/source/weibo \
			$(WEIBO_FOLDER)/source/hash \
			$(WEIBO_FOLDER)/source/oauth \
			$(WEIBO_FOLDER)/source/xmalloc
			
			

# multiple  例如  /usr/include   /user/openssl
INCLUDE_PATH    := . common http_stack  $(API_FOLD)/include  $(HTTP_FOLD)/include $(PARSER_FOLDER)/include $(WEIBO_FOLDER)/include

# multiple
#LIB_PATH        := . libs/linux
LIB_PATH        := . 

# multiple  例如：pthread  rt  
#LIBS            := osp expat pthread rt ssl crypto dl 
LIBS            := pthread

# 打包archive(.a)还是exe(.out)
MODE            := ARCHIVE

# 最终输出的文件名称
TARGET          := ../../10-common/lib/release/$(TARGET_DIR)/libmicroblog.a

# 带完整路径的object集合
FULL_OBJS       := $(foreach obj, $(OBJS), $(obj).o)

# object集合依赖的.d文件集合
DEPS            := $(foreach obj, $(OBJS), $(obj).d)

INCLUDE_PATH_FLAGS   := $(foreach obj, $(INCLUDE_PATH), -I$(obj) )
LIB_PATH_FLAGS       := $(foreach obj, $(LIB_PATH), -L$(obj) )
LIBS_FLAGS           := $(foreach obj, $(LIBS), -l$(obj) )

# 编译器选择
CC              := $(CROSS)gcc
AR              := $(CROSS)ar


DEFINE        := __LINUX__ 
# 其他选项，例如：宏 -D_LINUX_，优化 -O2/-g
ifeq ($(LINUX_COMPILER),_ARM_)
DEFINE        := __LINUX__  __ARM_D6446__
endif

ifeq ($(LINUX_COMPILER),_DAVINCI_)
DEFINE        := __LINUX__  _NETRA600_
endif

DEFINE_FLAG   := $(foreach i, $(DEFINE), -D$(i) )
FLAGS         := $(DEFINE_FLAG)  -Wall -O2

	
# 编译可执行文件的
#all:$(DEPS) $(FULL_OBJS)
#	@$(CC) $(FLAGS) $(INCLUDE_PATH_FLAGS) $(LIB_PATH_FLAGS)  \
#	               $(FULL_OBJS) $(LIBS_FLAGS) -o $(TARGET);	
	
# 打包的
all:$(DEPS) $(FULL_OBJS)
	@$(AR) -r  $(TARGET) $(FULL_OBJS);
	
clean:
	@rm -f $(DEPS);
	@rm -f $(FULL_OBJS);
	@rm -f $(TARGET);

# 按 .cpp, .cc, .c的顺序排列	
%.d:%.cpp
	@rm -f $@ $@.*; \
	$(CC) -MM $(INCLUDE_PATH_FLAGS) $(DEFINE_FLAG) $< > $@.$$$$; \
	sed -e '$$a\\	@$(CC) $(FLAGS) $(INCLUDE_PATH_FLAGS) -c $< -o $(@:.d=.o)' -e '1s/^.*\.o/$(subst /,\/,$(@:.d=.o))/' < $@.$$$$ \
	> $@; \
	rm -f $@.$$$$;
	
%.d:%.cc
	@rm -f $@ $@.*; \
	$(CC) -MM $(INCLUDE_PATH_FLAGS) $(DEFINE_FLAG) $< > $@.$$$$; \
	sed -e '$$a\\	@$(CC) $(FLAGS) $(INCLUDE_PATH_FLAGS) -c $< -o $(@:.d=.o)' -e '1s/^.*\.o/$(subst /,\/,$(@:.d=.o))/' < $@.$$$$ \
	> $@; \
	rm -f $@.$$$$;
	
%.d:%.c
	@rm -f $@ $@.*; \
	$(CC) -MM $(INCLUDE_PATH_FLAGS) $(DEFINE_FLAG) $< > $@.$$$$; \
	sed -e '$$a\\	@$(CC) $(FLAGS) $(INCLUDE_PATH_FLAGS) -c $< -o $(@:.d=.o)' -e '1s/^.*\.o/$(subst /,\/,$(@:.d=.o))/' < $@.$$$$ \
	> $@; \
	rm -f $@.$$$$;
	
-include $(DEPS)
