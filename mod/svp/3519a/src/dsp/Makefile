# Hisilicon Hi35xx sample Makefile
include ../Makefile.param

CFLAGS += -I$(PWD)/sample
CFLAGS += -I$(PWD)/encapsulation/include

SMP_SRCS := $(wildcard *.c)
SMP_SRCS += $(wildcard ./sample/*.c)
SMP_SRCS += $(wildcard ./encapsulation/src/*.c)
SMP_SRCS += $(wildcard $(PWD)/../common/*.c)
TARGET := sample_dsp_main

TARGET_PATH := $(PWD)

# compile linux or HuaweiLite
include $(PWD)/../../$(ARM_ARCH)_$(OSTYPE).mak
