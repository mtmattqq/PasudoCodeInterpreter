VPATH = src
objects = color.o pseudo.o shell.o
flag = -std=c++11

shell : $(objects)
	g++ $(objects) -o shell

shell.o : shell.cpp pseudo.o color.o
	g++ -c $(flag) shell.cpp

pseudo.o : pseudo.h pseudo.cpp color.o
	g++ -c $(flag) src/pseudo.cpp

color.o : color.h color.cpp
	g++ -c $(flag) src/color.cpp

clean : 
	rm -f *.o

run :
	make
	clear
	./shell

test : test/test_inheritance.cpp test/test_unicode.cpp
	g++ test/test_inheritance.cpp -o test_inheritance
	./test_inheritance