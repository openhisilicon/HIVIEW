
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
LDFLAGS := -g $(shell echo ${GSF_LDFLAGS})
TARG := bin/webs.exe

#============================================================= 

#WEBRTC
RTC_INCS :=  -DWEBRTC_ENABLE
RTC_LIBS := -lkvsWebrtcClient -lkvspic -lsrtp2 -lusrsctp

INCS := -I$(HOME) -Isrc -Iinc \
        -I$(HOME)/fw/openssl/openssl-1.1.0l \
        -DMG_ENABLE_HTTP_STREAMING_MULTIPART \
        -I$(HOME)/fw/kvs/inc $(RTC_INCS)
#SRCS := $(shell ls src/*.c)
SRCS := $(shell find src -type f -name "*.c" -o -name "*.cpp")
OBJS := $(patsubst %.c, %.o, $(SRCS))


LIBS += -L$(HOME)/lib/$(GSF_CPU_ARCH) $(RTC_LIBS) \
        -lflv -lcfifo -lpthread -lrt -lnm -lcjson -lcomm -lssl -lcrypto -lm

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
