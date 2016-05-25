all: test1 test2

test1: stack.o main.o
	gcc -ggdb -o test1 stack.o main.o

test2: stack.o main2.o
	gcc -ggdb -o test2 stack.o main2.o

stack.o: stack.c
	gcc -ggdb -c stack.c

main.o: main.c
	gcc -ggdb -c main.c

main2.o: main2.c
	gcc -ggdb -c main2.c

dep:
	@echo "Do nothing"

clean:
	rm -f test1 test2 *.o

