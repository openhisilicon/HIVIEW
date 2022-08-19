
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
LDFLAGS := -g $(shell echo ${GSF_LDFLAGS}) $(shell echo ${GSF_EXECFLAGS})
TARG := bin/app.exe

#============================================================= 

INCS := -I$(HOME) -Isrc -Iinc \
        -I$(HOME)/fw/comm/inc 
        
#SRCS := $(shell ls src/*.c)
SRCS := $(shell find src -type f -name "*.c" -o -name "*.cpp")
OBJS := $(patsubst %.c, %.o, $(SRCS))

LIBS += -L$(HOME)/lib/$(GSF_CPU_ARCH) -Llittlevgl -llittlevgl -llvgl -lcomm -lcfifo -lrt -lnm -lcjson -lm -lpthread

#FreeType
#export EXTRA_CFLAGS = -DEXTRA_FREETYPE
#LIBS += -lfreetype


$(TARG): $(OBJS)
	@$(MAKE) -C littlevgl || exit "$$?"
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	cp $(TARG) $(HOME)/bin/$(GSF_CPU_ARCH)/ -v

DEPS := $(patsubst %.o, %.d, $(OBJS))
-include $(DEPS)
.c.o:
	$(CC) $(CFLAGS) -c -MMD $< $(INCS) -o $@
  
.Phony: clean
clean:
	$(MAKE) -C littlevgl clean
	-rm $(TARG) $(OBJS) $(DEPS) src/*.bak -rf
