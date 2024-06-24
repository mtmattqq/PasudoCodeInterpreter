VPATH = src
objects = color.o position.o token.o node.o parser.o lexer.o symboltable.o interpreter.o pseudo.o shell.o 
flag = -std=c++11

shell : $(objects)
	g++ $(objects) -o shell

shell.o :
	g++ -c $(flag) shell.cpp

color.o :
	g++ -c $(flag) src/color.cpp

position.o :
	g++ -c $(flag) src/position.cpp

token.o :
	g++ -c $(flag) src/token.cpp

node.o : token.o
	g++ -c $(flag) src/node.cpp

parser.o : lexer.o color.o token.o
	g++ -c $(flag) src/parser.cpp

lexer.o : color.o token.o
	g++ -c $(flag) src/lexer.cpp

symboltable.o : 
	g++ -c $(flag) src/symboltable.cpp

interpreter.o : value.h symboltable.o node.o token.o
	g++ -c $(flag) src/interpreter.cpp

pseudo.o : value.h color.o node.o
	g++ -c $(flag) src/pseudo.cpp

clean : 
	rm -f *.o
	rm -f shell

run :
	make
	clear
	./shell

test : test/test_inheritance.cpp test/test_unicode.cpp
	g++ test/test_inheritance.cpp -o test_inheritance
	./test_inheritance