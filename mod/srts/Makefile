
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
LDFLAGS := -g $(shell echo ${GSF_LDFLAGS})
TARG := bin/srts.exe

#============================================================= 

INCS := -I$(HOME) -Isrc -Iinc \
        -I$(HOME)/fw/comm/inc \
        -I$(HOME)/fw/libmpeg/inc \
        -I$(HOME)/fw/librtp/inc  \
        -I$(HOME)/fw/srtp/srtp2
        
#SRCS := $(shell ls src/*.c)
SUBDIRS=$(shell find ./src/ -maxdepth 7 -type d)
SRCS:=$(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.c))

OBJS := $(patsubst %.c, %.o, $(SRCS))
LIBS += -L$(HOME)/lib/$(GSF_CPU_ARCH) -lsrtp2 -lssl -lcrypto -lrtp -lmpeg -lcfifo -lnm -lcjson -lm -lpthread -lrt

$(TARG): SJB $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	cp $(TARG) $(HOME)/bin/$(GSF_CPU_ARCH)/ -v

.c.o:
	$(CC) $(CFLAGS) -c $< $(INCS) -o $@

SJB:
	touch src/sjb_srts.c;touch src/cfg.c
  
.Phony: clean
clean:
	-rm $(TARG) $(OBJS) src/*.bak -rf
