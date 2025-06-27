CC = cc
CXX = c++
LD = ld
INCLUDE = 
LIBS = 

OBJ = a_string.o a_vector.o
HEADERS = a_common.h a_string.h a_vector.h

RELEASE_CFLAGS = -O2 -Wall -Wextra -pedantic -march=native $(INCLUDE) $(LIBS)
DEBUG_CFLAGS = -O0 -g -Wall -Wextra -pedantic -fsanitize=address $(INCLUDE) $(LIBS)

TARGET=release

ifeq ($(TARGET),release)
	CFLAGS=$(RELEASE_CFLAGS)
else
	CFLAGS=$(DEBUG_CFLAGS)
endif

build: setup $(HEADERS) $(OBJ)
	ld -r $(OBJ) -o asv.o

setup: deps

deps:

clean:
	rm -rf $(OBJ) asv.*

cleanall: clean defaults

.PHONY: clean cleanall
