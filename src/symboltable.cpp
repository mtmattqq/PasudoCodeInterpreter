/// --------------------
/// SymbolTable
/// --------------------

#include "symboltable.h"
#include "color.h"
#include <memory>

std::shared_ptr<Value> SymbolTable::get(std::string name) {
    if(symbols.count(name) == 0){
        if(parent != nullptr) {
            return parent->get(name);
        } else if(BUILTIN_ALGOS.count(name)) {
            return BUILTIN_ALGOS.at(name);
        } else {
            return std::make_shared<ErrorValue>(
                VALUE_ERROR, Color(0xFF, 0x39, 0x6E).get() + "Identifier: \""+ name +"\" has not defined\n" RESET);
        }
    }
    return symbols[name];
}

void SymbolTable::set(std::string name, std::shared_ptr<Value> value) {
    symbols[name] = value;
}

void SymbolTable::erase(std::string name) {
    symbols.erase(name);
}