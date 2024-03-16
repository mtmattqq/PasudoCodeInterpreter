VPATH = src
objects = color.o position.o token.o node.o parser.o lexer.o symboltable.o interpreter.o pseudo.o shell.o 
flag = -std=c++11 -g

shell : $(objects)
	g++ $(objects) -o shell

shell.o : shell.cpp
	g++ -c $(flag) shell.cpp

color.o : color.h color.cpp
	g++ -c $(flag) src/color.cpp

position.o : position.h position.cpp
	g++ -c $(flag) src/position.cpp

token.o : token.h token.cpp color.o position.o
	g++ -c $(flag) src/token.cpp

node.o : node.h node.cpp token.o
	g++ -c $(flag) src/node.cpp

parser.o : parser.h parser.cpp lexer.o
	g++ -c $(flag) src/parser.cpp

lexer.o : lexer.h lexer.cpp color.h
	g++ -c $(flag) src/lexer.cpp

symboltable.o : symboltable.h symboltable.cpp color.o node.o value.h
	g++ -c $(flag) src/symboltable.cpp

interpreter.o : interpreter.h interpreter.cpp value.h
	g++ -c $(flag) src/interpreter.cpp

pseudo.o : pseudo.h pseudo.cpp color.o position.o token.o node.o parser.o lexer.o symboltable.o interpreter.o
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