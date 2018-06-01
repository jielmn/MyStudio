# 标准makefile文件

# ==============================================================
#              请修改这部分的内容
# omit .o  包括完整的路径，如 $(BASE)/t1 $(CALL)/call
OBJS            := 


# 宏定义
DEFINES         := 


# multiple  例如  /usr/include   /user/openssl
INCLUDE_PATH    := .


# multiple
LIB_PATH        := .


# multiple  例如：pthread  rt  
LIBS            := 


# 打包archive(.a)还是exe(.out)
MODE            := ARCHIVE


# 最终输出的文件名称
TARGET          := libtarget.a


# 编译器选择
CC              := g++
AR              := ar


# 其他选项，例如：-Wall -g
MISC_FLAGS           := -Wall -g

# ==================================================================================






# ==============================================================================
#                 以下不需要修改，以来于以上的参数

# 带完整路径的object集合
FULL_OBJS       := $(foreach obj, $(OBJS), $(obj).o)

# object集合依赖的.d文件集合
DEPS            := $(foreach obj, $(OBJS), $(obj).d)

DEFINE_FLAG          := $(foreach obj, $(DEFINES), -D$(obj) )
INCLUDE_PATH_FLAGS   := $(foreach obj, $(INCLUDE_PATH), -I$(obj) )
LIB_PATH_FLAGS       := $(foreach obj, $(LIB_PATH), -L$(obj) )
LIBS_FLAGS           := $(foreach obj, $(LIBS), -l$(obj) )







# =================================================================================
#                           主体部分

# 编译可执行文件的
.PHONY : clean
all:$(DEPS) $(FULL_OBJS)
ifeq ($(MODE), ARCHIVE)
	@$(AR) -r  $(TARGET) $(FULL_OBJS);
else
	@$(CC) $(DEFINE_FLAG) $(MISC_FLAGS) $(INCLUDE_PATH_FLAGS) $(LIB_PATH_FLAGS)  \
	       $(FULL_OBJS) $(LIBS_FLAGS) -o $(TARGET);
endif
	
.PHONY : clean
clean:
	@rm -f $(DEPS);
	@rm -f $(FULL_OBJS);
	@rm -f $(TARGET);





# 按 .cpp, .cc, .c的顺序排列	
%.d:%.cpp
	@rm -f $@ $@.*; \
	$(CC) -MM $(DEFINE_FLAG) $(INCLUDE_PATH_FLAGS) $< > $@.$$$$; \
	sed -e '$$a\\	@$(CC) $(DEFINE_FLAG) $(MISC_FLAGS) $(INCLUDE_PATH_FLAGS) -c $< -o $(@:.d=.o)' -e '1s/^.*\.o/$(subst /,\/,$(@:.d=.o))/' < $@.$$$$ \
	> $@; \
	rm -f $@.$$$$;
	
%.d:%.cc
	@rm -f $@ $@.*; \
	$(CC) -MM $(DEFINE_FLAG) $(INCLUDE_PATH_FLAGS)  $< > $@.$$$$; \
	sed -e '$$a\\	@$(CC) $(DEFINE_FLAG) $(MISC_FLAGS) $(INCLUDE_PATH_FLAGS) -c $< -o $(@:.d=.o)' -e '1s/^.*\.o/$(subst /,\/,$(@:.d=.o))/' < $@.$$$$ \
	> $@; \
	rm -f $@.$$$$;
	
%.d:%.c
	@rm -f $@ $@.*; \
	$(CC) -MM $(DEFINE_FLAG) $(INCLUDE_PATH_FLAGS)  $< > $@.$$$$; \
	sed -e '$$a\\	@$(CC) $(DEFINE_FLAG) $(MISC_FLAGS) $(INCLUDE_PATH_FLAGS) -c $< -o $(@:.d=.o)' -e '1s/^.*\.o/$(subst /,\/,$(@:.d=.o))/' < $@.$$$$ \
	> $@; \
	rm -f $@.$$$$;
	
-include $(DEPS)
