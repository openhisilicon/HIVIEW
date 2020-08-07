
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
LDFLAGS := -g $(shell echo ${GSF_LDFLAGS})
TARG := bin/gb28181.exe

#============================================================= 

INCS := -I$(HOME) -Isrc -Iinc  \
        -I$(HOME)/fw/gb28181/inc \
        -I$(HOME)/fw/comm/inc 
        
#SRCS := $(shell ls src/*.c)
SRCS := $(shell find src -type f -name "*.c" -o -name "*.cpp")
OBJS := $(patsubst %.c, %.o, $(SRCS))

LIBS += -L$(HOME)/lib/$(GSF_CPU_ARCH) -lGB28181Device -leXosip2 -losip2 -losipparser2 -lresolv -ldl -lrt -lnm -lcjson -lm -lpthread

$(TARG): SJB $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	cp $(TARG) $(HOME)/bin/$(GSF_CPU_ARCH)/ -v

.c.o:
	$(CC) $(CFLAGS) -c $< $(INCS) -o $@

SJB:
	touch src/sjb_gb28181.c;touch src/cfg.c
  
.Phony: clean
clean:
	-rm $(TARG) $(OBJS) src/*.bak -rf
