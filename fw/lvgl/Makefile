#
# Makefile
#

#CC ?= arm-hisiv510-linux-gcc
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})

HOME := $(shell echo ${GSF_HOME})

LVGL_DIR ?= ${shell pwd}
CFLAGS ?= -Wall -Wshadow -Wundef -Wmaybe-uninitialized -O3 -g0 -I$(LVGL_DIR)/ -I$(HOME)
CFLAGS += -I${GSF_HOME}/fw/freetype/inc
LDFLAGS ?= -lpthread -lfreetype
BIN = liblvgl.a

CFLAGS += $(GSF_CFLAGS)

#Collect the files to compile
MAINSRC = ./main.c ./mouse_ico.c

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk
#include $(LVGL_DIR)/lv_examples/lv_examples.mk

OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))


SRCS = $(ASRCS) $(CSRCS) 
OBJS = $(AOBJS) $(COBJS)



all: default

%.o: %.c
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"
    
#$(CC) -o $(BIN)  $(AOBJS) $(COBJS) $(LDFLAGS)
default: $(AOBJS) $(COBJS) 
	$(AR) -rcs $(BIN)  $(AOBJS) $(COBJS)
	cp $(BIN) $(HOME)/lib/$(GSF_CPU_ARCH)/ -v
clean: 
	rm -f $(BIN) $(AOBJS) $(COBJS)

