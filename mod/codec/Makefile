
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
LDFLAGS := -g $(shell echo ${GSF_LDFLAGS}) $(shell echo ${GSF_EXECFLAGS})
TARG := bin/codec.exe
GSF_DEV_TYPE ?= ipc

#CFLAGS += -D__EYEM__
#CFLAGS += -D__GUIDE__
#CFLAGS += -D__FRM_PHY__
#============================================================= 

INCS := -I$(HOME) -I$(GSF_DEV_TYPE) -Iinc \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc/common \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc/hisisdk \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc/scene_auto
        
#SRCS := $(shell ls $(GSF_DEV_TYPE)/*.c)
SRCS := $(shell find $(GSF_DEV_TYPE) -type f -name "*.c" -o -name "*.cpp")
OBJS := $(patsubst %.c, %.o, $(SRCS))

LIBS += -L$(HOME)/lib/$(GSF_CPU_ARCH) -lcomm -lpthread -lfont -lmpp -lcfifo -lrt -lnm -lcjson -lm
         
ifeq ($(GSF_CPU_ARCH), 3516a)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -laf -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -l_hicalcflicker -l_hiacs -l_hiir_auto -ltde
else ifeq ($(GSF_CPU_ARCH), 3519)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -l_hiaf -l_hidefog \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -ltde
else ifeq ($(GSF_CPU_ARCH), 3516d)
#LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -laf -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
#        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -ltde
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -laf -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -l_hicalcflicker -l_hiir_auto -ltde
else ifeq ($(GSF_CPU_ARCH), 3519a)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -lhive_RES -laacenc -laacdec -ltde
else ifeq ($(GSF_CPU_ARCH), 3559a)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -lhive_RES -laacenc -laacdec -ltde -lhiavslut -lpos_query -lstdc++
else ifeq ($(GSF_CPU_ARCH), 3536d)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lmpi -lhdmi -laacenc -laacdec -ljpeg6b -ltde -lVoiceEngine -ldnvqe -lupvqe \
        -lpthread
else ifeq ($(GSF_CPU_ARCH), 3516e)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -ltde
else ifeq ($(GSF_CPU_ARCH), 3559)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiir_auto -l_hiacs -l_hiae -l_hiawb -laf -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -l_hicalcflicker -ltde
else ifeq ($(GSF_CPU_ARCH), 3531d)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lhdmi -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -live -lmd -lnnie -lhive_RES -laacenc -laacdec -laaccomm -ltde -lstdc++ 
else ifeq ($(GSF_CPU_ARCH), 3403)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lhdmi -lsecurec \
        -lisp -lot_isp -l_hiae -l_hiawb -l_hidehaze -l_hiextend_stats -l_hidrc -l_hildci -l_hibnr -l_hicalcflicker -l_hiir_auto -l_hiacs -l_hicrb \
        -live -lmpi -live -lmd -ltde -lVoiceEngine -ldnvqe -lupvqe -laac_enc -laac_dec -laac_comm -lpqp -lhnr -lmcf -l_mcf_vi -lstdc++ 
endif

$(TARG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	cp $(TARG) $(HOME)/bin/$(GSF_CPU_ARCH)/ -v

DEPS := $(patsubst %.o, %.d, $(OBJS))
-include $(DEPS)
.c.o:
	$(CC) $(CFLAGS) -c -MMD $< $(INCS) -o $@

.Phony: clean
clean:
	-rm $(TARG) $(OBJS) $(DEPS) $(GSF_DEV_TYPE)/*.bak -rf
