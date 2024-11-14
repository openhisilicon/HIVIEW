
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
LDFLAGS := -g $(shell echo ${GSF_LDFLAGS}) $(shell echo ${GSF_EXECFLAGS})
TARG := bin/webs.exe

#============================================================= 

#WEBRTC
RTC_INCS :=
RTC_LIBS :=

ifeq ($(GSF_CPU_ARCH), 3516a)
RTC_INCS +=  -DWEBRTC_ENABLE
RTC_LIBS += -lkvsWebrtcClient -lkvspic -lsrtp2 -lusrsctp
else ifeq ($(GSF_CPU_ARCH), 3516d)
RTC_INCS +=  -DWEBRTC_ENABLE
RTC_LIBS += -lkvsWebrtcClient -lkvspic -lsrtp2 -lusrsctp
endif


INCS := -I$(HOME) -Isrc -Iinc \
        -DMG_ENABLE_HTTP_STREAMING_MULTIPART \
        -I$(HOME)/fw/openssl/openssl-1.1.0l  \
        -I$(HOME)/fw/kvs/inc $(RTC_INCS) \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc/hisisdk

#SRCS := $(shell ls src/*.c)
SRCS := $(shell find src -type f -name "*.c" -o -name "*.cpp")
OBJS := $(patsubst %.c, %.o, $(SRCS))

LIBS += -L$(HOME)/lib/$(GSF_CPU_ARCH) $(RTC_LIBS) \
        -lflv -lcfifo -lpthread -lrt -lnm -lcjson -lcomm -lssl -lcrypto -lm

#__FRM_PHY__
#CFLAGS += -D__FRM_PHY__
#CFLAGS += -D__DMA_COPY__
#LIBS += $(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/libmppex.a -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -ltde -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -lsecurec


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
