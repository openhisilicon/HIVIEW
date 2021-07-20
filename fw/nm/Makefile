RM := rm
CP := cp
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
LDFLAGS := -shared -g $(shell echo ${GSF_LDFLAGS})
TARG := lib/libnm.so

#============================================================= 

INCS := -Isrc -Iinc
SRCS := $(shell ls src/*.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

#LIBS += -lpthread -lanl
LIBS += -lpthread

ifeq ($(GSF_CPU_ARCH), x86)
LIBS += -lanl
else ifeq ($(GSF_CPU_ARCH), 3516d)
LIBS += -lanl
else ifeq ($(GSF_CPU_ARCH), 3519a)
LIBS += -lanl
else ifeq ($(GSF_CPU_ARCH), 3559a)
LIBS += -lanl
else ifeq ($(GSF_CPU_ARCH), 3531d)
LIBS += -lanl
endif

DEP_OBJS := OBJS
DEP_LIBS += $(GSF_CPU_ARCH)/libnnm.a

$(TARG): $(OBJS)
	$(RM) lib/$(DEP_OBJS) -rf
	$(foreach a, $(DEP_LIBS), cd lib; $(AR) -t $(a) | sed 's/^/lib\//' >> $(DEP_OBJS); $(AR) x $(a); cd ..;)
	${CC} ${LDFLAGS} -o $@ ${OBJS} `cat lib/$(DEP_OBJS)` ${LIBS}
	rm -rf `cat lib/$(DEP_OBJS)`; rm -rf lib/$(DEP_OBJS);
	cp $(TARG) $(HOME)/lib/$(GSF_CPU_ARCH)/ -v

.c.o:
	$(CC) $(CFLAGS) -c $< $(INCS) -o $@

.Phony: clean
clean:
	-rm $(TARG) $(OBJS) src/*.bak -rf
