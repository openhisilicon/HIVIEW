RM := rm
CP := cp
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
CFLAGS += -DNN_USE_PIPE=1 -DNN_USE_EPOLL=1 -DNN_HAVE_POLL=1 -DNN_HAVE_MSG_CONTROL -DNN_HAVE_UNIX_SOCKETS
LDFLAGS := -shared -g $(shell echo ${GSF_LDFLAGS})
TARG := lib/libnm.so

#============================================================= 

INCS := -Isrc -Iinc -I.
SRCS := $(shell ls src/*.c)
SRCS += $(shell ls nanomsg/src/core/*.c)
SRCS += $(shell ls nanomsg/src/utils/*.c)
SRCS += $(shell ls nanomsg/src/aio/*.c)
SRCS += $(shell ls nanomsg/src/transports/utils/*.c)
SRCS += $(shell ls nanomsg/src/transports/tcp/*.c)
SRCS += $(shell ls nanomsg/src/transports/ws/*.c)
SRCS += $(shell ls nanomsg/src/transports/ipc/*.c)
SRCS += $(shell ls nanomsg/src/transports/inproc/*.c)
SRCS += $(shell ls nanomsg/src/protocols/utils/*.c)
SRCS += $(shell ls nanomsg/src/protocols/pipeline/*.c)
SRCS += $(shell ls nanomsg/src/protocols/pubsub/*.c)
SRCS += $(shell ls nanomsg/src/protocols/reqrep/*.c)
SRCS += $(shell ls nanomsg/src/protocols/survey/*.c)
SRCS += $(shell ls nanomsg/src/protocols/pair/*.c)
SRCS += $(shell ls nanomsg/src/protocols/bus/*.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

#LIBS += -lpthread -lanl
LIBS += -lpthread

#ifeq ($(GSF_CPU_ARCH), x86)
#LIBS += -lanl
#else ifeq ($(GSF_CPU_ARCH), 3516d)
#LIBS += -lanl
#else ifeq ($(GSF_CPU_ARCH), 3516a)
#LIBS += -lanl
#else ifeq ($(GSF_CPU_ARCH), 3519a)
#LIBS += -lanl
#else ifeq ($(GSF_CPU_ARCH), 3559a)
#LIBS += -lanl
#else ifeq ($(GSF_CPU_ARCH), 3531d)
#LIBS += -lanl
#endif

DEP_OBJS := OBJS
#DEP_LIBS += $(GSF_CPU_ARCH)/libnnm.a

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
