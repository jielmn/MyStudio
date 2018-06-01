# ��׼makefile�ļ�

# ==============================================================
#              ���޸��ⲿ�ֵ�����
# omit .o  ����������·������ $(BASE)/t1 $(CALL)/call
OBJS            := 


# �궨��
DEFINES         := 


# multiple  ����  /usr/include   /user/openssl
INCLUDE_PATH    := .


# multiple
LIB_PATH        := .


# multiple  ���磺pthread  rt  
LIBS            := 


# ���archive(.a)����exe(.out)
MODE            := ARCHIVE


# ����������ļ�����
TARGET          := libtarget.a


# ������ѡ��
CC              := g++
AR              := ar


# ����ѡ����磺-Wall -g
MISC_FLAGS           := -Wall -g

# ==================================================================================






# ==============================================================================
#                 ���²���Ҫ�޸ģ����������ϵĲ���

# ������·����object����
FULL_OBJS       := $(foreach obj, $(OBJS), $(obj).o)

# object����������.d�ļ�����
DEPS            := $(foreach obj, $(OBJS), $(obj).d)

DEFINE_FLAG          := $(foreach obj, $(DEFINES), -D$(obj) )
INCLUDE_PATH_FLAGS   := $(foreach obj, $(INCLUDE_PATH), -I$(obj) )
LIB_PATH_FLAGS       := $(foreach obj, $(LIB_PATH), -L$(obj) )
LIBS_FLAGS           := $(foreach obj, $(LIBS), -l$(obj) )







# =================================================================================
#                           ���岿��

# �����ִ���ļ���
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
	
-include $(DEPS)