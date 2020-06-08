#
# Makefile
#

#CC ?= arm-hisiv510-linux-gcc
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})

LVGL_DIR ?= ${shell pwd}
CFLAGS ?= -Wall -Wshadow -Wundef -Wmaybe-uninitialized -O3 -g0 -I$(LVGL_DIR)/
LDFLAGS ?= -lpthread
BIN = liblittlevgl.a


#Collect the files to compile
MAINSRC = ./main.c ./mouse_ico.c

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk
include $(LVGL_DIR)/lv_examples/lv_examples.mk

OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))

MAINOBJ = $(MAINSRC:.c=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS) $(MAINSRC)
OBJS = $(AOBJS) $(COBJS)

## MAINOBJ -> OBJFILES

all: default

%.o: %.c
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"
    
#$(CC) -o $(BIN) $(MAINOBJ) $(AOBJS) $(COBJS) $(LDFLAGS)
default: $(AOBJS) $(COBJS) $(MAINOBJ)
	$(AR) -rcs $(BIN) $(MAINOBJ) $(AOBJS) $(COBJS) 
clean: 
	rm -f $(BIN) $(AOBJS) $(COBJS) $(MAINOBJ)

