# Makefile for all libs
# (c) fangyi, 2014

GSF_HOME ?= $(PWD)
GSF_CPU_ARCH ?= XXX


MOD_DIRS := mod/bsp   \
			mod/mpp/${GSF_CPU_ARCH} \
			mod/codec \
			mod/rtsps \
			mod/rec   \
			mod/webs  \
			mod/onvif \
			mod/svp   \
			mod/rtmps \
			mod/sips  \
			mod/srts  \
			mod/app 

FW_DIRS :=  fw/nm       \
			fw/cfifo    \
			fw/cjson    \
			fw/mxml     \
			fw/comm     \
			fw/tindex   \
			fw/libmov   \
			fw/libfont  \
			fw/libhttp  \
			fw/librtp   \
			fw/librtsp  \
			fw/librtmp  \
			fw/libmpeg  \
			fw/libsip   \
			fw/libflv   \
			fw/h26xbits \
			fw/gsoap    \
			fw/lvgl 

CLEAN_DIRS := $(addprefix _cls_, $(FW_DIRS) $(MOD_DIRS))

.PHONY: mod fw $(FW_DIRS) $(MOD_DIRS) CHECK_ENV SUMMARY clean

mod: CHECK_ENV $(MOD_DIRS) SUMMARY
	@echo "..."

fw: CHECK_ENV $(FW_DIRS) SUMMARY
	@echo "..."

SUMMARY:
	@echo ""
	@echo "================ GSF-$(GSF_DEV_TYPE) ================"
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

$(MOD_DIRS):
	@$(MAKE) -C $@ || exit "$$?"

$(FW_DIRS):
	@$(MAKE) -C $@ || exit "$$?"
	
clean: $(CLEAN_DIRS)
	#-rm $(GSF_HOME)/bin/$(GSF_CPU_ARCH)/*.exe -rf
	#-rm $(GSF_HOME)/lib/$(GSF_CPU_ARCH)/*.so -rf

$(CLEAN_DIRS):
	$(MAKE) -C $(patsubst _cls_%, %, $@) clean
