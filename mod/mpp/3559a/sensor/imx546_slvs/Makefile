# Build: source build/3559a && make -C mod/mpp/3559a/sensor/imx546_slvs
RM := rm -f
HOME := $(shell echo $${GSF_HOME})
CC := $(shell echo $${GSF_CC})
CFLAGS := $(shell echo $${GSF_CFLAGS})
LDFLAGS := -shared -fPIC $(shell echo $${GSF_LDFLAGS})

INCS := -I$(HOME)/mod/mpp/3559a/inc/hisisdk \
        -I$(HOME)/mod/mpp/3559a/inc/common \
        -I$(HOME)/mod/mpp/3559a/inc \
        -I.

SRCS := imx546_sensor_ctl.c imx546_cmos.c
OBJS := $(SRCS:.c=.o)
TARG := libsns_imx546_slvs.so

LIBS := -L$(HOME)/mod/mpp/3559a/lib/hisisdk -lisp -l_hiae -l_hiawb -lmpi -lsecurec

all: $(TARG)
	cp -f $(TARG) $(HOME)/mod/mpp/3559a/lib/hisisdk/
	cp -f $(TARG) $(HOME)/lib/$(GSF_CPU_ARCH)/ 2>/dev/null || true
	@echo "built $(TARG)"

$(TARG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< $(INCS) -o $@

clean:
	$(RM) $(OBJS) $(TARG)
