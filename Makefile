CXX=g++
RM=rm -f
CPPFLAGS=-g -Wall -std=c++17 -D_REENTRANT -I/usr/include/SDL2
LDLIBS=-lSDL2 -lSDL2_ttf
LDFLAGS=-g
SRCS=bedside.cc model.cc
OBJS=$(subst .cc,.o,$(SRCS))

all: bedside

bedside: $(OBJS)
	$(CXX) $(LDFLAGS) -o bedside.app $(OBJS) $(LDLIBS)

bedside.o: bedside.cc model.h

model.o: model.cc model.h

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) bedside
