all: vector

vector: main.o Trace.o Vector.o
	g++ -o $@ $^

%.o: %.cpp
	g++ -Wall $(CFLAGS) -o $@ -c $<

dep: 
	g++ -Wall $(CFLAGS) -MM *.cpp > dep.txt

clean:
	rm -f *.o vector


