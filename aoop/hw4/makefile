all: mylex

mylex: main.o lexer.o
	g++ -o $@ $^

%.o: %.cpp
	g++ -Wall -o $@ -c $<

dep: 
	g++ -MM *.cpp > dep.txt

clean:
	rm -f *.o mylex
