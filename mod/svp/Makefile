
HOME := $(shell echo ${GSF_HOME})
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})
CFLAGS := $(shell echo ${GSF_CFLAGS})
CXXFLAGS := -std=c++11 ${CFLAGS}
LDFLAGS := -g $(shell echo ${GSF_LDFLAGS}) $(shell echo ${GSF_EXECFLAGS})
TARG := bin/svp.exe

#============================================================= 

INCS := -I$(HOME) -I$(GSF_CPU_ARCH) -Iinc -I$(GSF_CPU_ARCH)/common \
        -I$(GSF_CPU_ARCH)/ive/sample -I$(GSF_CPU_ARCH)/nnie/sample -I$(GSF_CPU_ARCH)/nnie/sample_nnie_software \
        -I$(GSF_CPU_ARCH)/dsp/encapsulation/include -I$(GSF_CPU_ARCH)/dsp/sample \
        -I$(GSF_CPU_ARCH)/dpu -I$(GSF_CPU_ARCH)/ive -I$(GSF_CPU_ARCH)/svp_npu \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc/hisisdk \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc/common \
        -I$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/inc \
        -I$(HOME)/fw/comm/inc \
        -I$(HOME)/fw/sqlite/inc \
        -I$(HOME)/fw/zbar/inc

ifeq ($(GSF_CPU_ARCH), 3519d)
        INCS += -I$(HOME)/fw/opencv/inc.4.5
else ifeq ($(GSF_CPU_ARCH), 3519d_003)
        INCS += -I$(HOME)/fw/opencv/inc.4.5
else
        INCS += -I$(HOME)/fw/opencv/inc
endif

        
#SRCS := $(shell ls src/*.c)
SRCS := $(shell find $(GSF_CPU_ARCH) -type f -name "*.c" -o -name "*.cpp")
#OBJS := $(patsubst %.c, %.o, $(SRCS))
OBJS = $(addsuffix .o, $(basename $(SRCS)))

LIBS += -L$(HOME)/lib/$(GSF_CPU_ARCH) -lmpp -lrt -lnm -lcjson -lm -lpthread

ifeq ($(GSF_CPU_ARCH), 3516a)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -live -lmd -lnnie -llpr -lstdc++ -l_hicalcflicker -l_hiir_auto -l_hiacs \
        -lm -ldl -lyolov5 -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lsqlite3 -lzbar -ltde
else ifeq ($(GSF_CPU_ARCH), 3519)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -l_hiaf -l_hidefog \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -ltde
else ifeq ($(GSF_CPU_ARCH), 3516d)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -live -lmd -lnnie -llpr -lstdc++ -l_hicalcflicker -l_hiir_auto \
        -lm -ldl -lyolov5 -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lsqlite3 -lzbar -ltde
else ifeq ($(GSF_CPU_ARCH), 3519a)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -live -lmd -lnnie -lhive_RES -laacenc -laacdec -ldsp -ltde
else ifeq ($(GSF_CPU_ARCH), 3559a)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -live -lmd -lnnie -lhive_RES -laacenc -laacdec -ldsp -ltde -lhiavslut -lpos_query -lstdc++ 
else ifeq ($(GSF_CPU_ARCH), 3516e)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiae -l_hiawb -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -ltde
else ifeq ($(GSF_CPU_ARCH), 3559)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lisp -l_hiir_auto -l_hiacs -l_hiae -l_hiawb -lhdmi -l_hidrc -l_hildci -l_hidehaze -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -live -lmd -lnnie -llpr -lstdc++  -l_hicalcflicker -ltde \
        -lm -ldl -lyolov5 -lopencv_core -lopencv_imgcodecs -lopencv_imgproc
else ifeq ($(GSF_CPU_ARCH), 3531d)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lhdmi -lsecurec \
        -live -lmpi -lVoiceEngine -ldnvqe -lupvqe -live -lmd -lnnie -lhive_RES -laacenc -laacdec -laaccomm -ltde -lstdc++ 
else ifeq ($(GSF_CPU_ARCH), 3403)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lhdmi -lsecurec \
        -lisp -lot_isp -l_hiae -l_hiawb -l_hidehaze -l_hiextend_stats -l_hidrc -l_hildci -l_hibnr -l_hicalcflicker -l_hiir_auto -l_hiacs -l_hicrb -lfileformat -lheif \
        -live -lmpi -live -lmd -ltde -lVoiceEngine -ldnvqe -lupvqe -laac_enc -laac_dec -laac_comm -lpqp -lhnr -lmcf -l_mcf_vi -lstdc++ \
        -lm -ldl -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk/npu/stub -lascendcl -ldsp -lyolov5 -lyolov8tiny -lopencv_core -lopencv_imgcodecs -lopencv_imgproc 
else ifeq ($(GSF_CPU_ARCH), 3519d_003)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lsecurec \
        -lhi_mpi -lhi_mpi_sysbind -lhi_mpi_sysmem -lhi_mpi_ae -lhi_mpi_isp -lot_mpi_isp -lhi_mpi_awb -ldehaze -lextend_stats -ldrc -lldci -lbnr -lcalcflicker -lir_auto -lacs \
        -lhi_mpi_aibnr -lhi_mpi_aidrc -lhi_mpi_ai3dnr -lhi_mpi_mcf -lsvp_acl -lhi_mpi_km -lhi_mpi_thermo -lprotobuf-c -lheif -lfileformat \
        -lhi_mpi_audio -lvoice_engine -lupvqe -ldnvqe -lhi_mpi_audio_adp -lopus -lvqe_res -lvqe_record -lvqe_hpf -lvqe_anr -lvqe_aec -lvqe_agc -lvqe_eq -lvqe_talkv2 \
        -laac_dec -laac_enc -laac_comm -laac_sbr_dec -laac_sbr_enc -lstdc++ -lhi_tde -lhi_mpi_ive -lmp3_dec -lmp3_enc -lmp3_lame \
        -lopencv_core -lopencv_imgcodecs -lopencv_imgproc
else ifeq ($(GSF_CPU_ARCH), 3519d)
LIBS += -L$(HOME)/mod/mpp/$(GSF_CPU_ARCH)/lib/hisisdk -lsecurec \
        -lhi_mpi -lhi_mpi_sysbind -lhi_mpi_sysmem -lhi_mpi_ae -lhi_mpi_isp -lot_mpi_isp -lhi_mpi_awb -ldehaze -lextend_stats -ldrc -lldci -lbnr -lcalcflicker -lir_auto -lacs \
        -lhi_mpi_aibnr -lhi_mpi_aidrc -lhi_mpi_ai3dnr -lhi_mpi_mcf -lsvp_acl -lhi_mpi_km -lhi_mpi_thermo -lprotobuf-c -lheif -lfileformat \
        -lhi_mpi_audio -lvoice_engine -lupvqe -ldnvqe -lhi_mpi_audio_adp -lopus -lvqe_res -lvqe_record -lvqe_hpf -lvqe_anr -lvqe_aec -lvqe_agc -lvqe_eq -lvqe_talkv2 \
        -laac_dec -laac_enc -laac_comm -laac_sbr_dec -laac_sbr_enc -lstdc++ -lhi_tde -lhi_mpi_ive -lmp3_dec -lmp3_enc -lmp3_lame \
        -lopencv_core -lopencv_imgcodecs -lopencv_imgproc
endif


$(TARG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	cp $(TARG) $(HOME)/bin/$(GSF_CPU_ARCH)/ -v

DEPS := $(patsubst %.o, %.d, $(OBJS))
-include $(DEPS)
.c.o:
	$(CC) $(CFLAGS) -c -MMD $< $(INCS) -o $@

.cpp.o:
	$(CC) $(CXXFLAGS) -c -MMD $< $(INCS) -o $@
  
.Phony: clean
clean:
	-rm $(TARG) $(OBJS) $(DEPS) $(GSF_CPU_ARCH)/*.bak -rf
