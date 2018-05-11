CC = g++

OPT =
CFLAGS = -std=c++11 -O2 -g
#LDFLAGS = -L/usr/local/lib 
LDFLAGS =
#LIBS = -lpfring -ldag -lpcap -lrt
LIBS = -lpthread
DEF = 
#INCLUDE = -I/usr/include/mysql -I/usr/local/include -I/usr/local/include/mysql++ -I../include -I./
INCLUDE = -I/usr/local/include -I../include -I./

TARGET = torinet
LIB_A = lib$(TARGET).a

OBJS = $(SRCS:.cpp=.o)
#SRCS = TcpSession.cpp UdpSession.cpp IpcSession.cpp TcpServer.cpp IoServicePool.cpp
SRCS = TcpSession.cpp TcpServer.cpp IoServicePool.cpp
#SRCS = TcpSession.cpp

all: $(LIB_A)
$(LIB_A) : $(OBJS)
	ar rc $(LIB_A) $(OBJS)

%.o : %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(DEF) $(LDFLAGS) $(LIBS) -c -o $@ $<

install :
	cp $(LIB_A) /usr/lib/

clean:
	rm -rf *.o $(LIB_A)

