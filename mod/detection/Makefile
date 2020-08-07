
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
LDFLAGS := -g $(shell echo ${GSF_LDFLAGS})
TARG := bin/detection.exe

#============================================================= 

INCS := -I$(HOME) -Isrc -Iinc  \
        -I$(HOME)/fw/comm/inc  \
		-I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc/common \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc/hisisdk \
        
#SRCS := $(shell ls src/*.c)
SRCS := $(shell find src -type f -name "*.c" -o -name "*.cpp")
OBJS := $(patsubst %.c, %.o, $(SRCS))

LD_OPENCV_LIBS += -lopencv_highgui -lopencv_imgproc -lopencv_dnn -lopencv_core -lopencv_imgcodecs
LIBS += -L$(HOME)/lib/$(GSF_CPU_ARCH) -lresolv -ldl -lrt -lnm -lcjson -lm -lpthread -lins_nnie
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -live -lmd -lnnie -lstdc++  -l_hicalcflicker
LIBS += $(LD_OPENCV_LIBS)
$(TARG): SJB $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	cp $(TARG) $(HOME)/bin/$(GSF_CPU_ARCH)/ -v

.c.o:
	$(CC) $(CFLAGS) -c $< $(INCS) -o $@

SJB:
	touch src/sjb_detection.c;touch src/cfg.c
  
.Phony: clean
clean:
	-rm $(TARG) $(OBJS) src/*.bak -rf
