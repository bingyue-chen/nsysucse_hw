#compiler
CC = g++
#CC = g++

#cflags
CFLAGS = -g -ansi

#object

all: b013040040_server b013040040_client

b013040040_server: server_socket.o server.o config.o
	$(CC) $(CFLAGS) server_socket.o server.o config.o -o $@

b013040040_client: client_socket.o client.o config.o
	$(CC) $(CFLAGS) client_socket.o client.o config.o -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm *.o

