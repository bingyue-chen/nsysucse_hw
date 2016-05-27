#compiler
#CC = gcc
CC = g++

FILES = $(wildcard *.cpp)

OBJFILES = $(FILES:.cpp=.o)

all: my_pagereference

my_pagereference: $(OBJFILES)
	$(CC) -o $@ $(OBJFILES)

clean:
	rm -f *.o my_pagereference

dep:
	$(CC) -MM *.c > dep.txt
