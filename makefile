# hw08
EXE=hw08

# Main target
all: $(EXE)

#  Msys/MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -fopenmp -DUSEGLEW
LIBS=-lglfw3 -lglew32 -lglu32 -lopengl32 -lm
CLEAN=rm -f *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations  -DUSEGLEW
LIBS=-lglfw -lglew -framework Cocoa -framework OpenGL -framework IOKit
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall -fopenmp
LIBS=-lglfw -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

x239lib/CSCIx239.a:
	cd x239lib && $(MAKE)

# Dependencies
hw08.o: hw08.c x239lib/CSCIx239.h
perlin.o: perlin.c

# Compile rules
.c.o:
	gcc -c $(CFLG) $<
.cpp.o:
	g++ -c $(CFLG) $<

#  Link
hw08:hw08.o   perlin.o x239lib/CSCIx239.a
	g++ $(CFLG) -o $@ $^ $(LIBS)

#  Clean
clean:
	cd x239lib && $(CLEAN)
	$(CLEAN)