# Hisilicon Hi35xx sample Makefile
include ../Makefile.param

.PHONY: lib_clean

#TARGET := $(SRCS:%.c=%)
SUBDIRS := ./src/sensor/sensor*

all:
	@mkdir $(shell pwd)/out/sensor_bin -p	
	for SENSOR in $(SUBDIRS);do\
		make -C $$SENSOR;done

clean: sensor_clean

sensor_clean:
	for SENSOR in $(SUBDIRS);do\
		make clean -C $$SENSOR;done

