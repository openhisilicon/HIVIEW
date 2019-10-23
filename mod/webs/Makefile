
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
LDFLAGS := -g $(shell echo ${GSF_LDFLAGS})
TARG := bin/webs.exe

#============================================================= 

INCS := -I$(HOME) -Isrc -Iinc -I$(HOME)/fw/libwebsockets/inc -DMG_ENABLE_HTTP_STREAMING_MULTIPART
#SRCS := $(shell ls src/*.c)
SRCS := $(shell find src -type f -name "*.c" -o -name "*.cpp")
OBJS := $(patsubst %.c, %.o, $(SRCS))

LIBS += -L$(HOME)/lib/$(GSF_CPU_ARCH) -lflv -lcfifo -lpthread -lrt -lnm -lcjson -lm

$(TARG): SJB $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	cp $(TARG) $(HOME)/bin/$(GSF_CPU_ARCH)/ -v

.c.o:
	$(CC) $(CFLAGS) -c $< $(INCS) -o $@

SJB:
	touch src/sjb_webs.c;touch src/webs.c
  
.Phony: clean
clean:
	-rm $(TARG) $(OBJS) src/*.bak -rf
