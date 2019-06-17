# Hisilicon Hi3519V101 sample Makefile

include ../Makefile.param

SRCS += $(wildcard *.c ./src/common/*.c) 
SRCS += $(wildcard *.c ./src/iniparser/*.c)
CFLAGS +=-I./src/iniparser
CFLAGS += -I./src/include -I$(REL_INC) -I$(SDK_PATH)/mpp/code/include/inner_inc
TARGET  := sample_scene

# compile linux or HuaweiLite
include $(PWD)/../Make.$(OSTYPE)


