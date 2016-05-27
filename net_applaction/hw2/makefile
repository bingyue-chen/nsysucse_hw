#compiler
CC = g++
#CC = g++

#cflags
CFLAGS = -g -ansi -O3

#object

all: b013040040_server.exe b013040040_client.exe

b013040040_server.exe: huffmancode.o server_socket.o server.o
	$(CC) $(CFLAGS) huffmancode.o server_socket.o server.o  -o $@

b013040040_client.exe: huffmancode.o client_socket.o client.o
	$(CC) $(CFLAGS) huffmancode.o client_socket.o client.o  -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm *.o 
dep:
	$(CC) -MM *.c > dep.txt

