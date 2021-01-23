CPPFLAGS=-arch x86_64 -m64
DEBUG=-g -DDEBUG
OPT=-O2

all: clean mazegen

mazegen: mazegen.cpp
	$(value CXX) mazegen.cpp $(CPPFLAGS) $(INCLUDE) $(OPT) -o mazegen
	$(value CXX) mazegen.cpp $(DEBUG) $(CPPFLAGS) $(INCLUDE) -o mazegen-debug
	strip -no_uuid -A -u -S -X -N -x mazegen

clean:
	rm -f ./mazegen
	rm -f ./*-debug
	rm -f *.*~
	rm -f *~

