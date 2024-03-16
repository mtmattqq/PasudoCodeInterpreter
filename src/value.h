/// --------------------
/// Value
/// --------------------

#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <vector>
#include <memory>
#include "node.h"

const std::string VALUE_NONE{"NONE"};
const std::string VALUE_INT{"Int"};
const std::string VALUE_FLOAT{"Float"};
const std::string VALUE_ALGO{"Algo"};
const std::string VALUE_STRING{"Str"};
const std::string VALUE_ERROR{"ERROR"};
const std::string VALUE_ARRAY{"Array"};

class SymbolTable;
class Interpreter;
class Value {
public:
    Value(const std::string& _type = VALUE_NONE)
        : type(_type) {}
    virtual std::string get_num() { return type;}
    virtual std::string get_type(){ return type;}
    virtual std::shared_ptr<Value> execute(NodeList args = {}, SymbolTable *parent = nullptr)
        { return std::make_shared<Value>();};
    friend std::ostream& operator<<(std::ostream &out, Value &token);
    
protected:
    std::string type;
};

using ValueList = std::vector<std::shared_ptr<Value>>;

template<typename T>
class TypedValue: public Value {
public:
    TypedValue(const std::string& _type, const T &_value) 
        : Value(_type), value(_value) {}
    virtual std::string get_num();

protected:
    T value;
};

using ErrorValue = TypedValue<std::string>;

class BaseAlgoValue: public Value {
public:
    BaseAlgoValue(const std::string &_algo_name, std::shared_ptr<Node> _value) 
        : Value(VALUE_ALGO), value(_value), algo_name(_algo_name) {}
    virtual std::string get_num() { return algo_name;}
    virtual std::shared_ptr<Value> set_args(NodeList&, SymbolTable&, Interpreter&);
protected:
    std::string algo_name;
    std::shared_ptr<Node> value;
};

class AlgoValue: public BaseAlgoValue {
public:
    AlgoValue(const std::string &_algo_name, std::shared_ptr<Node> _value) 
        : BaseAlgoValue(_algo_name, _value) {}
    virtual std::string get_num() { return algo_name;}
    virtual std::shared_ptr<Value> execute(NodeList args = {}, SymbolTable *parent = nullptr) override;
protected:
};

class BuiltinAlgoValue: public BaseAlgoValue {
public:
    BuiltinAlgoValue(const std::string &_algo_name, std::shared_ptr<Node> _value) 
        : BaseAlgoValue(_algo_name, _value) {}
    virtual std::string get_num() { return algo_name;}
    virtual std::shared_ptr<Value> execute(NodeList args = {}, SymbolTable *parent = nullptr) override;
    std::shared_ptr<Value> execute_print(const std::string&);
    std::shared_ptr<Value> execute_read();
    std::shared_ptr<Value> execute_read_line();
    std::shared_ptr<Value> execute_clear();
    std::shared_ptr<Value> execute_int(const std::string&);
    std::shared_ptr<Value> execute_float(const std::string&);
    std::shared_ptr<Value> execute_string(const std::string&);
protected:
};

class ArrayValue: public Value {
public:
    ArrayValue(ValueList _value) 
        : Value(VALUE_ARRAY), value(_value) {}
    virtual std::string get_num();
    std::shared_ptr<Value>& operator[](int p);
    void push_back(std::shared_ptr<Value>);
    std::shared_ptr<Value> pop_back();
protected:
    ValueList value;
    std::shared_ptr<Value> error;
};

std::shared_ptr<Value> operator+(std::shared_ptr<Value>, std::shared_ptr<Value>);
std::shared_ptr<Value> operator-(std::shared_ptr<Value>, std::shared_ptr<Value>);
std::shared_ptr<Value> operator*(std::shared_ptr<Value>, std::shared_ptr<Value>);
std::shared_ptr<Value> operator/(std::shared_ptr<Value>, std::shared_ptr<Value>);
std::shared_ptr<Value> operator%(std::shared_ptr<Value>, std::shared_ptr<Value>);
std::shared_ptr<Value> pow(std::shared_ptr<Value>, std::shared_ptr<Value>);

std::shared_ptr<Value> operator==(std::shared_ptr<Value>, std::shared_ptr<Value>);
std::shared_ptr<Value> operator!=(std::shared_ptr<Value>, std::shared_ptr<Value>);
std::shared_ptr<Value> operator<(std::shared_ptr<Value>, std::shared_ptr<Value>);
std::shared_ptr<Value> operator>(std::shared_ptr<Value>, std::shared_ptr<Value>);
std::shared_ptr<Value> operator<=(std::shared_ptr<Value>, std::shared_ptr<Value>);
std::shared_ptr<Value> operator>=(std::shared_ptr<Value>, std::shared_ptr<Value>);

std::shared_ptr<Value> operator&&(std::shared_ptr<Value>, std::shared_ptr<Value>);
std::shared_ptr<Value> operator||(std::shared_ptr<Value>, std::shared_ptr<Value>);

std::shared_ptr<Value> operator-(std::shared_ptr<Value>);
std::shared_ptr<Value> operator!(std::shared_ptr<Value>);

template class TypedValue<double>;
template class TypedValue<int64_t>;
template class TypedValue<std::string>;

#endif