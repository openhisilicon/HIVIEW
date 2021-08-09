
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
LDFLAGS := -g $(shell echo ${GSF_LDFLAGS})
TARG := bin/sips.exe

#============================================================= 

INCS := -I$(HOME) -Isrc -Iinc \
        -I$(HOME)/fw/libst/inc \
        -I$(HOME)/fw/libhttp/inc \
        -I$(HOME)/fw/librtp/inc \
        -I$(HOME)/fw/librtsp/inc \
        -I$(HOME)/fw/h26xbits/inc \
        -I$(HOME)/fw/libsip/inc \
        -I$(HOME)/fw/libmpeg/inc
        
#SRCS := $(shell ls src/*.c)
SRCS := $(shell find src -type f -name "*.c" -o -name "*.cpp")
OBJS := $(patsubst %.c, %.o, $(SRCS))

LIBS += -L$(HOME)/lib/$(GSF_CPU_ARCH) -lh26xbits -lsip -lrtsp -lrtp -lmpeg -lhttp -lst -lcfifo -lrt -lnm -lmxml -lcjson -lcomm -lm -lpthread

$(TARG): SJB $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	cp $(TARG) $(HOME)/bin/$(GSF_CPU_ARCH)/ -v

.c.o:
	$(CC) $(CFLAGS) -c $< $(INCS) -o $@

SJB:
	touch src/sjb_sips.c;touch src/cfg.c
	touch src/sxb_sips.c;touch src/uas-app.c
  
.Phony: clean
clean:
	-rm $(TARG) $(OBJS) src/*.bak -rf
