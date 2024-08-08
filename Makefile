VPATH = src
objects = color.o position.o token.o node.o parser.o lexer.o symboltable.o interpreter.o pseudo.o shell.o 
flag = -std=c++11

shell : $(objects)
	g++ $(objects) -o shell

shell.o :
	g++ -c $(flag) shell.cpp

color.o : src/color.cpp src/color.h
	g++ -c $(flag) src/color.cpp

position.o : src/position.cpp src/position.h
	g++ -c $(flag) src/position.cpp

token.o : src/token.cpp src/token.h
	g++ -c $(flag) src/token.cpp

node.o : token.o src/node.cpp src/node.h
	g++ -c $(flag) src/node.cpp

parser.o : lexer.o color.o token.o src/parser.cpp src/parser.h
	g++ -c $(flag) src/parser.cpp

lexer.o : color.o token.o src/lexer.cpp src/lexer.h
	g++ -c $(flag) src/lexer.cpp

symboltable.o : src/symboltable.cpp src/symboltable.h
	g++ -c $(flag) src/symboltable.cpp

interpreter.o : value.h symboltable.o node.o token.o src/interpreter.cpp src/interpreter.h
	g++ -c $(flag) src/interpreter.cpp

pseudo.o : value.h color.o node.o src/pseudo.cpp src/pseudo.h
	g++ -c $(flag) src/pseudo.cpp

clean : 
	rm -f *.o
	rm -f shell

run :
	make
	clear
	./shell

test : test/test_inheritance.cpp test/test_unicode.cpp
	g++ test/test_inheritance.cpp -std=c++11 -o test_inheritance
	./test_inheritance