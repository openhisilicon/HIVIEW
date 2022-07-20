
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
LDFLAGS := -g $(shell echo ${GSF_LDFLAGS})
TARG := bin/rtsps.exe

#============================================================= 

INCS := -I$(HOME) -Isrc -Iinc -Isrc/st-rtsp \
        -I$(HOME)/fw/libst/inc \
        -I$(HOME)/fw/libhttp/inc \
        -I$(HOME)/fw/librtp/inc \
        -I$(HOME)/fw/librtsp/inc \
        -I$(HOME)/fw/h26xbits/inc \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc/hisisdk
        
#SRCS := $(shell ls src/*.c)
SRCS := $(shell find src -type f -name "*.c" -o -name "*.cpp")
OBJS := $(patsubst %.c, %.o, $(SRCS))

LIBS += -L$(HOME)/lib/$(GSF_CPU_ARCH) -lh26xbits -lrtsp -lrtp -lhttp -lst -lcfifo -lrt -lnm -lcjson -lm -lpthread

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
