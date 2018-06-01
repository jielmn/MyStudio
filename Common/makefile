# 标准makefile文件
# omit .o  包括完整的路径，如 $(BASE)/t1 $(CALL)/call
OBJS            := Common  LmnContainer  LmnString  StdAfx LmnMisc LmnSeli LmnTelnetd LmnFileUtil

# 宏定义
DEFINES         := DEBUG

# multiple  例如  /usr/include   /user/openssl
INCLUDE_PATH    := .

# multiple
LIB_PATH        := .

# multiple  例如：pthread  rt  
LIBS            := 

# 打包archive(.a)还是exe(.out)
MODE            := ARCHIVE

# 最终输出的文件名称
TARGET          := libLmnCommon.a

# 带完整路径的object集合
FULL_OBJS       := $(foreach obj, $(OBJS), $(obj).o)

# object集合依赖的.d文件集合
DEPS            := $(foreach obj, $(OBJS), $(obj).d)

DEFINE_FLAG          := $(foreach obj, $(DEFINES), -D$(obj) )
INCLUDE_PATH_FLAGS   := $(foreach obj, $(INCLUDE_PATH), -I$(obj) )
LIB_PATH_FLAGS       := $(foreach obj, $(LIB_PATH), -L$(obj) )
LIBS_FLAGS           := $(foreach obj, $(LIBS), -l$(obj) )

# 编译器选择
CC              := g++
AR              := ar

# 其他选项，例如：优化 -O2/-g
FLAGS           := -Wall -O2 -fno-strict-aliasing  $(DEFINE_FLAG)

# 编译可执行文件的
all:$(DEPS) $(FULL_OBJS)
ifeq ($(MODE), ARCHIVE)
	@$(AR) -r  $(TARGET) $(FULL_OBJS);
else
	@$(CC) $(FLAGS) $(DEFINE_FLAG) $(INCLUDE_PATH_FLAGS) $(LIB_PATH_FLAGS)  \
	       $(FULL_OBJS) $(LIBS_FLAGS) -o $(TARGET);
endif
	
#打包的
# all:$(DEPS) $(FULL_OBJS)
#	@$(AR) -r  $(TARGET) $(FULL_OBJS);
	
clean:
	@rm -f $(DEPS);
	@rm -f $(FULL_OBJS);
	@rm -f $(TARGET);

# 按 .cpp, .cc, .c的顺序排列	
%.d:%.cpp
	@rm -f $@ $@.*; \
	g++ -MM $(DEFINE_FLAG) $(INCLUDE_PATH_FLAGS) $< > $@.$$$$; \
	sed -e '$$a\\	@$(CC) $(FLAGS) $(INCLUDE_PATH_FLAGS) -c $< -o $(@:.d=.o)' -e '1s/^.*\.o/$(subst /,\/,$(@:.d=.o))/' < $@.$$$$ \
	> $@; \
	rm -f $@.$$$$;
	
%.d:%.cc
	@rm -f $@ $@.*; \
	g++ -MM $(DEFINE_FLAG) $(INCLUDE_PATH_FLAGS)  $< > $@.$$$$; \
	sed -e '$$a\\	@$(CC) $(FLAGS) $(INCLUDE_PATH_FLAGS) -c $< -o $(@:.d=.o)' -e '1s/^.*\.o/$(subst /,\/,$(@:.d=.o))/' < $@.$$$$ \
	> $@; \
	rm -f $@.$$$$;
	
%.d:%.c
	@rm -f $@ $@.*; \
	g++ -MM $(DEFINE_FLAG) $(INCLUDE_PATH_FLAGS)  $< > $@.$$$$; \
	sed -e '$$a\\	@$(CC) $(FLAGS) $(INCLUDE_PATH_FLAGS) -c $< -o $(@:.d=.o)' -e '1s/^.*\.o/$(subst /,\/,$(@:.d=.o))/' < $@.$$$$ \
	> $@; \
	rm -f $@.$$$$;
	
-include $(DEPS)
