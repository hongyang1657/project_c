#AudioLibBASE    = /home/$(USER)/SVNAudioLib/dev/audiodec/codec/AudioLib
#include $(AudioLibBASE)/config.mk
#CC=gcc
CC=arm-openwrt-linux-gnueabi-gcc
LIBPATH=./lib
BUILDPATH=./
STATIC_ADT=1
ifneq ($(LIBPATH),wildcard($(LIBPATH)))
a := $(shell mkdir -p $(LIBPATH))
endif
ifneq ($(BUILDPATH),wildcard($(BUILDPATH)))
a := $(shell mkdir -p $(BUILDPATH))
endif

CFLAGS=-Wall
CFLAGS+= -I./include
#CFLAGS+=-I../
CFLAGS+= -DCTOOL=2 -DLLVL=1 -DFIXED_POINT=16 -DLP_64
#CFLAGS+= -DCTOOL=2 -DLLVL=1 -DADD_SYNC_TONE -DFIXED_POINT=16
#CFLAGS+=$(EXTRA_FLAG)

$(warning "PREINCLUDE : $(PREINCLUDE)")
LIBSRC = \
	src/common \
	src/decode.c \
	src/encode.c \
	src/kiss_fft.c \
	src/rscode.c

ifeq ($(STATIC_ADT),1)
LIB=$(LIBPATH)/libADT.a
CFLAGS+=-fPIC
else
LIB=$(LIBPATH)/libADT.so
CFLAGS+=-fPIC 
CFLAGS+=-fno-stack-protector
endif


SourcePath = $(foreach src_dir, $(LIBSRC), $(dir $(src_dir)))
$(foreach src_dir, $(SourcePath), $(shell mkdir -p $(BUILDPATH)/$(src_dir)))

LIBOBJ = $(addprefix $(BUILDPATH)/, $(addsuffix .o ,$(basename $(LIBSRC))))

LoadFlags = -O3 -fPIC 

## define commands for make, sush as all, clean
.PHONY: all clean cleanall
all: $(LIB)

clean:
	rm -f $(LIB) $(LIBOBJ)

cleanall: clean
	-rm -rf $(BUILDPATH)

$(BUILDPATH)/%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

ifeq ($(STATIC_ADT),1)
$(LIB):$(LIBOBJ)
	$(AR) -rcs $@ $^
else
$(LIB):$(LIBOBJ)
	$(LD) -o $@ $^ $(LoadFlags) -shared
endif
