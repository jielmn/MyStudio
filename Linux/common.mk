
# ==============================================================================
#                 ���²���Ҫ�޸ģ������ڴ��ݹ����Ĳ���

# ������·����object����
FULL_OBJS       := $(foreach obj, $(OBJS), $(obj).o)

# object����������.d�ļ�����
DEPS            := $(foreach obj, $(OBJS), $(obj).d)

DEFINE_FLAG          := $(foreach obj, $(DEFINES), -D$(obj) )
INCLUDE_PATH_FLAGS   := $(foreach obj, $(INCLUDE_PATH), -I$(obj) )
LIB_PATH_FLAGS       := $(foreach obj, $(LIB_PATH), -L$(obj) )
LIBS_FLAGS           := $(foreach obj, $(LIBS), -l$(obj) )



ifeq ($(DEBUG), YES)
MISC_FLAGS = $(MISC_FLAGS) -g
else
MISC_FLAGS = $(MISC_FLAGS) -O2
endif




# =================================================================================
#                           ���岿��


# �����clean����
ifeq ($(COMMAND), clean)
.PHONY : clean
clean:
	@rm -f $(DEPS);
	@rm -f $(FULL_OBJS);
	@rm -f $(TARGET);
endif


# ����Ǳ���exe
ifeq ($(MODE), EXE)
all:$(DEPS) $(FULL_OBJS)
	@$(CC) $(DEFINE_FLAG) $(MISC_FLAGS) $(INCLUDE_PATH_FLAGS) $(LIB_PATH_FLAGS)  \
	       $(FULL_OBJS) $(LIBS_FLAGS) -o $(TARGET);
endif



# ����Ǳ���archive
ifeq ($(MODE), ARCHIVE)
all:$(DEPS) $(FULL_OBJS)
	@$(AR) -r  $(TARGET) $(FULL_OBJS);
endif













# �� .cpp, .cc, .c��˳������	
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
	
	
ifneq ($(COMMAND), clean)
-include $(DEPS)
endif
