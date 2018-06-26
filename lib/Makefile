CC = g++

OPT =
CFLAGS = -std=c++0x -O2
#LDFLAGS = -L/usr/local/lib 
LDFLAGS =
#LIBS = -lpfring -ldag -lpcap -lrt
LIBS =
DEF = 
#INCLUDE = -I/usr/include/mysql -I/usr/local/include -I/usr/local/include/mysql++ -I../include -I./
INCLUDE = -I/usr/local/include -I../include -I./

TARGET = torinet
LIB_A = lib$(TARGET).a

OBJS = $(SRCS:.cpp=.o)
#SRCS = TcpSession.cpp UdpSession.cpp IpcSession.cpp TcpServer.cpp IoServicePool.cpp
SRCS = TcpSession.cpp IoServicePool.cpp TcpServer.cpp

all: $(LIB_A)
$(LIB_A) : $(OBJS)
	ar rc $(LIB_A) $(OBJS)

%.o : %.cpp
	$(CC) $(INCLUDE) $(CFLAGS) $(DEF) $(LDFLAGS) $(LIBS) -c -o $@ $<

install :
	cp $(LIB_A) /usr/lib/

clean:
	rm -rf *.o $(LIB_A)

