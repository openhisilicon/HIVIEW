# Hisilicon Hi3516A sample Makefile

include ../Makefile.param

# target source
SRC  := $(wildcard *.c) 
OBJ  := $(SRC:%.c=%.o)

SCENE_DIR := src/common src/adapt/$(HIARCH) 
SCENE_SRC := $(shell find $(SCENE_DIR) -name '*.c')
SCENE_OBJ := $(SCENE_SRC:%.c=%.o)

TARGET := $(OBJ:%.o=%)
.PHONY : clean all

all: $(TARGET)

MPI_LIBS := $(REL_LIB)/libmpi.a
MPI_LIBS += $(REL_LIB)/libive.a

CFLAGS += -Isrc/include -I$(REL_INC) -I$(SDK_PATH)/mpp/code/include/inner_inc

$(TARGET): %:%.o $(SCENE_OBJ)
	$(CC) $(CFLAGS) -lpthread -lm -o $@ $^ $(SENSOR_LIBS) $(MPI_LIBS) $(AUDIO_LIBA)

clean:
	@rm -f $(TARGET)
	@rm -f $(OBJ)
	@rm -f $(COMM_OBJ)
	@rm -f $(SCENE_OBJ)

