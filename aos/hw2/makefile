#compiler
#CC = gcc
CC = g++

SER_OBJ = server.o server_socket.o user_management.o file_management.o
CLI_OBJ = client.o client_socket.o

all: server client

server: $(SER_OBJ)
	$(CC) -o $@ $(SER_OBJ)

client: $(CLI_OBJ)
	$(CC) -o $@ $(CLI_OBJ)

clean:
	rm -f *.o server client

dep:
	$(CC) -MM *.c > dep.txt
