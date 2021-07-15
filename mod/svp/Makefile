
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
CXXFLAGS := -std=c++11 ${CFLAGS}
LDFLAGS := -g $(shell echo ${GSF_LDFLAGS})
TARG := bin/svp.exe

#============================================================= 

INCS := -I$(HOME) -I$(GSF_CPU_ARCH) -Iinc -I$(GSF_CPU_ARCH)/common -I$(GSF_CPU_ARCH)/ive/sample \
        -I$(GSF_CPU_ARCH)/nnie/sample -I$(GSF_CPU_ARCH)/nnie/sample_nnie_software \
        -I$(GSF_CPU_ARCH)/dsp/encapsulation/include -I$(GSF_CPU_ARCH)/dsp/sample \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc/hisisdk \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc/common \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc \
        -I$(HOME)/fw/comm/inc \
        -I$(HOME)/fw/opencv/inc \
        -I$(HOME)/fw/sqlite/inc \
        -I$(HOME)/fw/zbar/inc
        
#SRCS := $(shell ls src/*.c)
SRCS := $(shell find $(GSF_CPU_ARCH) -type f -name "*.c" -o -name "*.cpp")
#OBJS := $(patsubst %.c, %.o, $(SRCS))
OBJS = $(addsuffix .o, $(basename $(SRCS)))

LIBS += -L$(HOME)/lib/$(GSF_CPU_ARCH) -lmpp -lrt -lnm -lcjson -lm -lpthread

ifeq ($(GSF_CPU_ARCH), 3516a)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -live -lmd -lnnie -llpr -lstdc++ -l_hicalcflicker -l_hiir_auto -l_hiacs \
        -lm -ldl -lyolov5 -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lsqlite3 -lzbar
else ifeq ($(GSF_CPU_ARCH), 3519)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -l_hiaf -l_hidefog \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe
else ifeq ($(GSF_CPU_ARCH), 3516d)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -live -lmd -lnnie -llpr -lstdc++ -l_hicalcflicker -l_hiir_auto \
        -lm -ldl -lyolov5 -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lsqlite3 -lzbar
else ifeq ($(GSF_CPU_ARCH), 3519a)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -live -lmd -lnnie -lhive_RES -laacenc -laacdec -ldsp
else ifeq ($(GSF_CPU_ARCH), 3559a)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -live -lmd -lnnie -lhive_RES -laacenc -laacdec -ldsp -ltde -lhiavslut -lpos_query -lstdc++ 
else ifeq ($(GSF_CPU_ARCH), 3516e)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe
else ifeq ($(GSF_CPU_ARCH), 3559)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiir_auto -l_hiacs -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -live -lmd -lnnie -llpr -lstdc++  -l_hicalcflicker -ltde \
        -lm -ldl -lyolov5 -lopencv_core -lopencv_imgcodecs -lopencv_imgproc
endif


$(TARG): SJB $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	cp $(TARG) $(HOME)/bin/$(GSF_CPU_ARCH)/ -v

.c.o:
	$(CC) $(CFLAGS) -c $< $(INCS) -o $@

.cpp.o:
	$(CC) $(CXXFLAGS) -c $< $(INCS) -o $@

SJB:
	touch $(GSF_CPU_ARCH)/sjb_svp.c;touch $(GSF_CPU_ARCH)/cfg.c
  
.Phony: clean
clean:
	-rm $(TARG) $(OBJS) $(GSF_CPU_ARCH)/*.bak -rf
