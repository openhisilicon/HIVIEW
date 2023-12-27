#
# Makefile
#

#CC ?= arm-hisiv510-linux-gcc
CC := $(shell echo ${GSF_CC})
AR := $(shell echo ${GSF_AR})

HOME := $(shell echo ${GSF_HOME})
LVGL_LIB := $(HOME)/fw/lvgl
LVGL_DIR ?= ${shell pwd}

CFLAGS ?= -Wall -Wshadow -Wundef -Wmaybe-uninitialized -O3 -g0 -I$(LVGL_LIB)/ -I$(HOME)
LDFLAGS ?= -lpthread
BIN = liblittlevgl.a
CFLAGS += $(EXTRA_CFLAGS)
CFLAGS += $(GSF_CFLAGS)


#Collect the files to compile
MAINSRC = ./main.c ./mouse_ico.c

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

