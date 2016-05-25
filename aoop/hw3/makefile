all: mycomplex

mycomplex: main.o Complex.o
	g++ -o $@ $^

%.o: %.cpp
	g++ -Wall -o $@ -c $<

dep: 
	g++ -MM *.cpp > dep.txt

clean:
	rm -f *.o mycomplex run.output

test:
	./mycomplex > run.output && diff run.output Complex.output
