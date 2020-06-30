
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
LDFLAGS := -g $(shell echo ${GSF_LDFLAGS})
TARG := bin/codec.exe
GSF_DEV_TYPE ?= ipc
#============================================================= 

INCS := -I$(HOME) -I$(GSF_DEV_TYPE) -Iinc \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc/common \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc/hisisdk \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc/scene_auto
        
#SRCS := $(shell ls $(GSF_DEV_TYPE)/*.c)
SRCS := $(shell find $(GSF_DEV_TYPE) -type f -name "*.c" -o -name "*.cpp")
OBJS := $(patsubst %.c, %.o, $(SRCS))

LIBS += -L$(HOME)/lib/$(GSF_CPU_ARCH) -lpthread -lfont -lmpp -lcfifo -lrt -lnm -lcjson -lm
         
ifeq ($(GSF_CPU_ARCH), 3516a)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -l_hiaf -l_hidefog -l_iniparser \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe
else ifeq ($(GSF_CPU_ARCH), 3519)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -l_hiaf -l_hidefog \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe
else ifeq ($(GSF_CPU_ARCH), 3516d)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe
else ifeq ($(GSF_CPU_ARCH), 3519a)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -lhive_RES -laacenc -laacdec
else ifeq ($(GSF_CPU_ARCH), 3559a)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -lhive_RES -laacenc -laacdec -ltde -lhiavslut -lpos_query -lstdc++
else ifeq ($(GSF_CPU_ARCH), 3536d)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lmpi -lhdmi -laacenc -laacdec -ljpeg6b -ltde -lVoiceEngine -ldnvqe -lupvqe \
        -lpthread
else ifeq ($(GSF_CPU_ARCH), 3516e)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe
endif

$(TARG): SJB $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	cp $(TARG) $(HOME)/bin/$(GSF_CPU_ARCH)/ -v

.c.o:
	$(CC) $(CFLAGS) -c $< $(INCS) -o $@

SJB:
	touch $(GSF_DEV_TYPE)/sjb_codec.c;touch $(GSF_DEV_TYPE)/cfg.c
  
.Phony: clean
clean:
	-rm $(TARG) $(OBJS) $(GSF_DEV_TYPE)/*.bak -rf
