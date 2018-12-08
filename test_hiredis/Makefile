
# Default target.
TARGET = test

HOST_CC = g++
#HOST_OPT = -O3
HOST_OPT = -std=c++0x -O2

LFLAGS = -L/usr/local/lib -L../lib/
LIBS = -lhiredis
IFLAGS = -I/usr/local/include -I./
HOST_CFLAGS = -Wall -fno-strict-aliasing -g $(HOST_OPT) $(IFLAGS)

OBJS = $(SRCS:.cpp=.o)
SRCS = test.cpp

%.o: %.cpp
	$(HOST_CC) $(HOST_CFLAGS) -c -o $@ $<

test: $(OBJS)
	$(HOST_CC) $(HOST_CFLAGS) -o $@ $^ $(LFLAGS) $(LIBS)


all: $(TARGET)

clean:
	rm -rf $(OBJS) $(TARGET)

install:
	cp $(TARGET) ../bin/

