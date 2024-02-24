#include <iostream>
#include <iomanip>
#include <chrono>
#include "src/pseudo.h"
#include "src/color.h"

using time_point = std::chrono::steady_clock::time_point;

void RunShell(std::string file_name) {
    SymbolTable global_symbol_table;
    while(true) {
        std::cout << Color(0x34, 0xD3, 0xDE) << "Pseudo >> " RESET;
        std::string input;
        std::getline(std::cin, input);
        time_point start{std::chrono::steady_clock::now()};
        std::cout << Run(file_name, input, global_symbol_table) << "\n";
        time_point end{std::chrono::steady_clock::now()};
        int64_t time_cost{std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()};
        std::cout << "Execution time: " << time_cost << " ms\n";
    }
}

int main(int argc, char *args[]) {
    if(argc == 1) {
        RunShell("stdin");
    } else {
        RunShell(args[1]);
    }
    return 0;
}