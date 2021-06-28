#source object target
SOURCE	:= mainRead.cc
OBJS	:= mainRead.o
TARGET	:= read

#compile and lib parameter
CC		:= arm-linux-gnueabihf-g++ -std=c++11
NPULIBS := -lnpu -lgxdnn -lcJSON -lsqlite3 -lpthread -ldl
# TTSLIBS	:= -ltts -lwrap -lcap -lpthread -lpulse -lpulsecommon-10.0 -lpulse-simple -ldbus-1 -lsndfile
LDFLAGS := -L./lib
DEFINES	:= 
INCLUDE	:= -I./include
CFLAGS	:= ./*/*.c
CXXFLAGS:= ./fd_share_memory.cc

#link
$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(CXXFLAGS) $(INCLUDE) $(LDFLAGS) $(NPULIBS)

#compile
%.o : %.cc
	@echo ""
	$(CC) -c $< -o $@

.PHONY : clean compile
clean:
	rm -rf $(TARGET)
compile:
	make
	rm -rf *.o

