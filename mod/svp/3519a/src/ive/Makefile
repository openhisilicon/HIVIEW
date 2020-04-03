# Hisilicon Hi35xx sample Makefile

include ../Makefile.param

CFLAGS += -I$(PWD)/sample

SMP_SRCS := $(wildcard *.c)
SMP_SRCS += $(wildcard ./sample/*.c)
SMP_SRCS += $(wildcard $(PWD)/../common/*.c)
TARGET := sample_ive_main

TARGET_PATH := $(PWD)

# compile linux or HuaweiLite
include $(PWD)/../../$(ARM_ARCH)_$(OSTYPE).mak
