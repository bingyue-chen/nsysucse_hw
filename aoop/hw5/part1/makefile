all: form

form: main.o Form.o
	g++ -o $@ $^

%.o: %.cpp
	g++ -Wall -o $@ -c $<

dep: 
	g++ -Wall -MM *.cpp > dep.txt

clean:
	rm -f *.o form


