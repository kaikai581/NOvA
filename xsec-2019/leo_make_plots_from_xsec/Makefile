
PROGS = $(shell ls src/*.cpp | sed 's/\.cpp//' | sed 's/src\///')

INCLUDES = -I$(shell root-config --incdir) -I$(ANAPACKDIR)/include -I$(PPFX_DIR)/include -I$(DK2NU)/include

DEPLIBS=$(shell root-config --libs) -lEG #-L$(ANAPACKDIR)/lib -lanapack -L$(DK2NU_LIB) -ldk2nuTree -L$(PPFX_DIR)/lib -lppfx 

CC	=	g++
COPTS	=	-fPIC -DLINUX -O0  -g 
FLAGS   =       -g

all:    programs

programs: $(PROGS)
	echo making $(PROGS)

$(PROGS): % : src/%.o
	if [ ! -d bin ]; then mkdir -p bin; fi
	$(CC) -Wall -o bin/$@ $< $(DEPLIBS)

%.o: %.cpp
	$(CC) $(COPTS) $(INCLUDES) -c -o $@ $<


clean:  delobj delbin

delobj:
	-rm src/*.o 

delbin:
	if [ -d bin ]; then rm -rf bin; fi



