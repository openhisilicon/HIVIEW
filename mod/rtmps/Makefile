
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
LDFLAGS := -g $(shell echo ${GSF_LDFLAGS}) $(shell echo ${GSF_EXECFLAGS})
TARG := bin/rtmps.exe

#============================================================= 

INCS := -I$(HOME) -Isrc -Iinc \
        -I$(HOME)/fw/comm/inc \
        -I$(HOME)/fw/libflv/inc\
        -I$(HOME)/fw/librtmp/inc
        
#SRCS := $(shell ls src/*.c)
SUBDIRS=$(shell find ./src/ -maxdepth 7 -type d)
SRCS:=$(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.c))

OBJS := $(patsubst %.c, %.o, $(SRCS))
LIBS += -L$(HOME)/lib/$(GSF_CPU_ARCH) -lrtmp -lflv -lcfifo -lnm -lcjson -lm -lpthread -lrt

$(TARG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	cp $(TARG) $(HOME)/bin/$(GSF_CPU_ARCH)/ -v

DEPS := $(patsubst %.o, %.d, $(OBJS))
-include $(DEPS)
.c.o:
	$(CC) $(CFLAGS) -c -MMD $< $(INCS) -o $@
  
.Phony: clean
clean:
	-rm $(TARG) $(OBJS) $(DEPS) src/*.bak -rf
