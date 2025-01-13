VPATH = src
CC = g++
CPPFLAGS = -std=c++17 -O2
TARGET = shell
SRCS = src/color.cpp src/position.cpp src/token.cpp src/node.cpp src/parser.cpp src/lexer.cpp src/symboltable.cpp src/interpreter.cpp src/pseudo.cpp src/shell.cpp
BUILD_DIR = build
OBJS = $(SRCS:src/%.cpp=$(BUILD_DIR)/%.o)

$(TARGET): $(BUILD_DIR) $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/shell.o:	src/shell.cpp src/interpreter.h src/pseudo.h
	$(CC) -c $(CPPFLAGS) src/shell.cpp -o $@

$(BUILD_DIR)/color.o: src/color.cpp src/color.h
	$(CC) -c $(CPPFLAGS) src/color.cpp -o $@

$(BUILD_DIR)/position.o: src/position.cpp src/position.h
	$(CC) -c $(CPPFLAGS) src/position.cpp -o $@

$(BUILD_DIR)/token.o: src/token.cpp src/token.h
	$(CC) -c $(CPPFLAGS) src/token.cpp -o $@

$(BUILD_DIR)/node.o: src/node.cpp src/node.h
	$(CC) -c $(CPPFLAGS) src/node.cpp -o $@

$(BUILD_DIR)/parser.o: src/parser.cpp src/parser.h
	$(CC) -c $(CPPFLAGS) src/parser.cpp -o $@

$(BUILD_DIR)/lexer.o: src/lexer.cpp src/lexer.h
	$(CC) -c $(CPPFLAGS) src/lexer.cpp -o $@

$(BUILD_DIR)/symboltable.o: src/symboltable.cpp src/symboltable.h
	$(CC) -c $(CPPFLAGS) src/symboltable.cpp -o $@

$(BUILD_DIR)/interpreter.o: value.h src/interpreter.cpp src/interpreter.h
	$(CC) -c $(CPPFLAGS) src/interpreter.cpp -o $@

$(BUILD_DIR)/pseudo.o: value.h src/pseudo.cpp src/pseudo.h
	$(CC) -c $(CPPFLAGS) src/pseudo.cpp -o $@

run : $(TARGET)
	./shell

.PHONY: clean all
clean:
	rm -rf $(BUILD_DIR) $(TARGET)
all: clean $(TARGET)