CC := g++
all : server client
INCLUDE := -I./
LIBS := -lpthread

PROGRAM1 := server
PROGRAM2 := client

DDEBUG :=
CXXFLAGS := -g -Wall -D_REENTRANT $(DDEBUG)

OBJECT1 := ChatServer.o EpollServerSocket.o Socket.o ThreadReadWriteLock.o Epoll.o
OBJECT2 := ChatClient.o ClientSocket.o Socket.o

.cpp.o:
	$(CC) $(CXXFLAGS) $(INCLUDE) -c $<

server : $(OBJECT1)
	$(CC) -o $(PROGRAM1)  $(OBJECT1) $(LIBS)

client : $(OBJECT2)
	$(CC) -o $(PROGRAM2) $(OBJECT2) $(LIBS)



.PHONY : clean
clean:
	-rm -f $(OBJECT1) $(OBJECT2) $(PROGRAM1) $(PROGRAM2)
