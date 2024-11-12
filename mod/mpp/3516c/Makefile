RM := rm
CP := cp
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
LDFLAGS := -shared -g $(shell echo ${GSF_LDFLAGS})
TARG := lib/libmpp.so


#CFLAGS += -DHNR_SCENE_AUTO_USED
#CFLAGS += -DCONFIG_SUPPORT_SAMPLE_ROTATION
#CFLAGS += -DCONFIG_CRYPTO_CHIP_HI3519DV500
#============================================================= 

INCS := -I$(HOME) -Isrc -Iinc -Iinc/hisisdk -Iinc/common -Iinc/scene_auto

#SRCS := $(shell ls src/*.c)
SUBDIRS=$(shell find ./src/ -maxdepth 7 -type d)
SRCS:=$(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.c))
	  
#mppex;
SUBDIRS_EX=$(shell find ./mppex/ -maxdepth 7 -type d)
SRCS_EX:=$(foreach dir,$(SUBDIRS_EX),$(wildcard $(dir)/*.c))
OBJS_EX:= $(patsubst %.c, %.o, $(SRCS_EX))
TARG_EX:= lib/libmppex.a
OBJS := $(patsubst %.c, %.o, $(SRCS))
LIBS += 
DEP_OBJS := OBJS
DEP_LIBS += libmppex.a libhi_mpi_cipher.a libhi_mpi_km.a libhi_mpi_otp.a

$(TARG): $(OBJS) $(TARG_EX)
	$(RM) lib/$(DEP_OBJS) -rf
	$(foreach a, $(DEP_LIBS), cd lib; $(AR) -t $(a) | sed 's/^/lib\//' >> $(DEP_OBJS); $(AR) x $(a); cd ..;)
	${CC} ${LDFLAGS} -o $@ ${OBJS} `cat lib/$(DEP_OBJS)` ${LIBS}
	rm -rf `cat lib/$(DEP_OBJS)`; rm -rf lib/$(DEP_OBJS);
	cp $(TARG) $(HOME)/lib/$(GSF_CPU_ARCH)/ -v

$(TARG_EX): $(OBJS_EX)
	${AR} -rcs $@ ${OBJS_EX}

.c.o:
	$(CC) $(CFLAGS) -c $< $(INCS) -o $@

.Phony: clean
clean:
	-rm $(TARG) $(OBJS) $(OBJS_EX) src/*.bak -rf
