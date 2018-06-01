# ��׼makefile�ļ�
# omit .o  ����������·������ $(BASE)/t1 $(CALL)/call
OBJS            := Common  LmnContainer  LmnString  StdAfx LmnMisc LmnSeli LmnTelnetd LmnFileUtil

# �궨��
DEFINES         := DEBUG

# multiple  ����  /usr/include   /user/openssl
INCLUDE_PATH    := .

# multiple
LIB_PATH        := .

# multiple  ���磺pthread  rt  
LIBS            := 

# ���archive(.a)����exe(.out)
MODE            := ARCHIVE

# ����������ļ�����
TARGET          := libLmnCommon.a

# ������·����object����
FULL_OBJS       := $(foreach obj, $(OBJS), $(obj).o)

# object����������.d�ļ�����
DEPS            := $(foreach obj, $(OBJS), $(obj).d)

DEFINE_FLAG          := $(foreach obj, $(DEFINES), -D$(obj) )
INCLUDE_PATH_FLAGS   := $(foreach obj, $(INCLUDE_PATH), -I$(obj) )
LIB_PATH_FLAGS       := $(foreach obj, $(LIB_PATH), -L$(obj) )
LIBS_FLAGS           := $(foreach obj, $(LIBS), -l$(obj) )

# ������ѡ��
CC              := g++
AR              := ar

# ����ѡ����磺�Ż� -O2/-g
FLAGS           := -Wall -O2 -fno-strict-aliasing  $(DEFINE_FLAG)

# �����ִ���ļ���
all:$(DEPS) $(FULL_OBJS)
ifeq ($(MODE), ARCHIVE)
	@$(AR) -r  $(TARGET) $(FULL_OBJS);
else
	@$(CC) $(FLAGS) $(DEFINE_FLAG) $(INCLUDE_PATH_FLAGS) $(LIB_PATH_FLAGS)  \
	       $(FULL_OBJS) $(LIBS_FLAGS) -o $(TARGET);
endif
	
#�����
# all:$(DEPS) $(FULL_OBJS)
#	@$(AR) -r  $(TARGET) $(FULL_OBJS);
	
clean:
	@rm -f $(DEPS);
	@rm -f $(FULL_OBJS);
	@rm -f $(TARGET);

# �� .cpp, .cc, .c��˳������	
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
