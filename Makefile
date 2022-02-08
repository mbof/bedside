CXX=g++
RM=rm -f
CPPFLAGS=-g -Wall -std=c++17 -D_REENTRANT -I/usr/include/SDL2
LDLIBS=-lSDL2 -lSDL2_ttf
LDFLAGS=-g
SRCS=bedside.cc
OBJS=$(subst .cc,.o,$(SRCS))

all: bedside

bedside: $(OBJS)
	$(CXX) $(LDFLAGS) -o bedside.app bedside.o $(LDLIBS)

bedside.o: bedside.cc

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) bedside
