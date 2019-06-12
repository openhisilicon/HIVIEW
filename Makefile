# Makefile for all libs
# (c) fangyi, 2014

GSF_HOME ?= $(PWD)
GSF_CPU_ARCH ?= XXX

#SUB_DIRS := fw/nm fw/cfifo fw/cjson 
SUB_DIRS := \
            mod/bsp/lib/sadp mod/bsp/lib/upg mod/bsp \
            mod/mpp/${GSF_CPU_ARCH} \
            mod/cam \
            mod/codec \
            mod/rec/lib/tindex mod/rec/lib/libmov  mod/rec

CLEAN_DIRS := $(addprefix _cls_, $(SUB_DIRS))

.PHONY: ALL_SUB $(SUB_DIRS) CHECK_ENV clean

ALL_SUB: CHECK_ENV $(SUB_DIRS)
	@echo ""
	@echo "================ GSF-IPC ================"
	@echo "All Done."
	@echo "Env: $(GSF_CPU_ARCH)"
	@echo "Output: $(GSF_HOME)/lib/$(GSF_CPU_ARCH)"
	@echo "Output: `ls -l $(GSF_HOME)/lib/$(GSF_CPU_ARCH)`"
	@echo "Output: $(GSF_HOME)/bin/$(GSF_CPU_ARCH)"
	@echo "Output: `ls -l $(GSF_HOME)/bin/$(GSF_CPU_ARCH)`"

CHECK_ENV:
	@echo "================ GSF-IPC ================"
	@echo "Env: $(GSF_CPU_ARCH)"
ifeq ($(GSF_CPU_ARCH), XXX)
	@echo "Env Error."
	@exit 1
endif

$(SUB_DIRS):
	@$(MAKE) -C $@ || exit "$$?"
	
clean: $(CLEAN_DIRS)
	#-rm $(GSF_HOME)/bin/$(GSF_CPU_ARCH)/*.exe -rf
	#-rm $(GSF_HOME)/lib/$(GSF_CPU_ARCH)/*.so -rf

$(CLEAN_DIRS):
	$(MAKE) -C $(patsubst _cls_%, %, $@) clean
