CFLAGS += -I$(PWD)/src
CFLAGS += -I./third_party/hisi/include
CFLAGS += -L./third_party/hisi/lib

CXXFLAGS += -I$(PWD)/src
CXXFLAGS += -I./third_party/hisi/include
CXXFLAGS += -I./third_party/opencv4/include/opencv4
CXXFLAGS += -L./third_party/opencv4/lib
CXXFLAGS += -L./third_party/hisi/lib
LD_OPENCV_LIBS += -lopencv_highgui -lopencv_imgproc -lopencv_dnn -lopencv_core -lopencv_imgcodecs

LD_HISI_LIBS +=  -laacdec -laacenc -ldetail_ap \
-ldnvqe -ldpu_match -ldpu_rect -ldsp -lhdmi -lhdr_ap -lhi_cipher -lhiavslut -lhifisheyecalibrate \
-live -lmd -lmfnr_ap -lmpi -lmpi_photo -lpciv\
-lpos_query -lsecurec -lsfnr_ap -lsvpruntime -ltde -lupvqe -lVoiceEngine -lnnie

CXXFLAGS += ${LD_OPENCV_LIBS}
CXXFLAGS += -std=c++11
CXXFLAGS += -Wno-error -Wno-write-strings
CXXFLAGS += -fomit-frame-pointer -fstrict-aliasing -ffunction-sections -fdata-sections -ffast-math -fno-rtti -fno-exceptions -fpermissive
CXXFLAGS += -O3
CXXFLAGS += -g -fstack-protector -fPIC -DGSF_CPU_3516d

SRCS := $(wildcard ./src/*.c)
TARGET := libins_nnie.so

# target source

OBJS  = $(SRCS:%.c=%.o)

CXX = arm-himix200-linux-g++
CC = arm-himix200-linux-gcc
.PHONY : clean all

all: $(TARGET)

$(TARGET):  ./sample/sample_nnie_main.o ./src/ins_nnie_interface.o ./src/ins_nnie_c_interface.o $(OBJS) 
	$(CXX) $(CXXFLAGS) -lpthread -lm -ldl -o $@ $^ -Wl,--start-group ${LD_HISI_LIBS} -Wl,--end-group -fPIC -shared
	cp $(TARGET) /root/Desktop/NowWork/HiViewGitHub/HIVIEW/lib/3516d/

clean:
	@rm -f $(TARGET) ./sample/sample_nnie_main.o ./src/ins_nnie_interface.o ./src/util.o
	@rm -f $(OBJS)
