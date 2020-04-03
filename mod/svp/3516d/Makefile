
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
LDFLAGS := -g $(shell echo ${GSF_LDFLAGS})
TARG := bin/svp.exe

#============================================================= 

INCS := -I$(HOME) -Isrc -Iinc -Isrc/common -Isrc/ive/sample -Isrc/nnie/sample -Isrc/nnie/sample_nnie_software \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc/hisisdk \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc/common \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc \
        -I$(HOME)/fw/comm/inc
        
#SRCS := $(shell ls src/*.c)
SRCS := $(shell find src -type f -name "*.c" -o -name "*.cpp")
OBJS := $(patsubst %.c, %.o, $(SRCS))

LIBS += -L$(HOME)/lib/$(GSF_CPU_ARCH) -lmpp -lrt -lnm -lcjson -lm -lpthread

ifeq ($(GSF_CPU_ARCH), 3516a)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -l_hiaf -l_hidefog -l_iniparser \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe
else ifeq ($(GSF_CPU_ARCH), 3519)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -l_hiaf -l_hidefog \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe
else ifeq ($(GSF_CPU_ARCH), 3516d)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -live -lmd -lnnie
endif


$(TARG): SJB $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	cp $(TARG) $(HOME)/bin/$(GSF_CPU_ARCH)/ -v

.c.o:
	$(CC) $(CFLAGS) -c $< $(INCS) -o $@

SJB:
	touch src/sjb_svp.c;touch src/cfg.c
  
.Phony: clean
clean:
	-rm $(TARG) $(OBJS) src/*.bak -rf
