# Default target.
TARGET = client

HOST_CC = g++
#HOST_OPT = -O3
HOST_OPT = -O2

LFLAGS = -L/usr/local/lib -L../Lib/
LIBS = -llog4cxx -lboost_system -lboost_filesystem -lboost_program_options -lboost_thread -ltorinet -lpthread
IFLAGS = -I/usr/local/include -I./
HOST_CFLAGS = -std=c++0x -Wall -fno-strict-aliasing -g $(HOST_OPT) $(IFLAGS)

OBJS = $(SRCS:.cpp=.o)
SRCS = Client.cpp Cli.cpp main.cpp
#SRCS = Client.cpp main.cpp

%.o: %.cpp
	$(HOST_CC) $(HOST_CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(HOST_CC) $(HOST_CFLAGS) -o $@ $^ $(LFLAGS) $(LIBS)

all: $(TARGET)

clean:
	rm -rf $(OBJS) $(TARGET)

install:
	cp client ../bin/
