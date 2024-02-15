VPATH = src
objects = pseudo.o color.o shell.o

shell : $(objects)
	g++ $(objects) -o shell

shell.o : shell.cpp pseudo.o color.o
	g++ -c shell.cpp

pseudo.o : pseudo.h pseudo.cpp
	g++ -c src/pseudo.cpp

color.o : color.h color.cpp
	g++ -c src/color.cpp

clean : 
	rm -f *.o

run :
	make
	make clean
	./shell

test : test/test_inheritance.cpp
	g++ test/test_inheritance.cpp -o test_inheritance
	./test_inheritance