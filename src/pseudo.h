#ifndef PSEUDO_H
#define PSEUDO_H

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <map>
#include <set>

#define NONE 0

/// --------------------
/// Position
/// --------------------

struct Position {
    int index, line, column;
    std::string file_name;
    Position(int idx = 0, int ln = 0, int col = 0, const std::string& _file_name = "")
        : index(idx), line(ln), column(col), file_name(_file_name) {}
    void advance(char);
    std::string get_pos();
    friend std::ostream& operator<<(std::ostream &out, Position &pos);
};

/// --------------------
/// Token
/// --------------------

// Builtin
const std::string TOKEN_KEYWORD{"KEYWORD"};
const std::string TOKEN_IDENTIFIER{"IDENTIFIER"};
const std::string TOKEN_ASSIGN{"ASSIGN"};
const std::string TOKEN_BUILTIN_CONST{"BICONST"};
const std::string TOKEN_BUILTIN_ALGO{"BIALGO"};

const std::string TOKEN_INT{"INT"};
const std::string TOKEN_FLOAT{"FLOAT"};
// Arithmetic
const std::string TOKEN_ADD{"ADD"};
const std::string TOKEN_SUB{"SUB"};
const std::string TOKEN_MUL{"MUL"};
const std::string TOKEN_DIV{"DIV"};
const std::string TOKEN_MOD{"MOD"};
const std::string TOKEN_POW{"POW"};
const std::string TOKEN_LEFT_PAREN{"LPAREN"};
const std::string TOKEN_RIGHT_PAREN{"RPAREN"};
// Comparison
const std::string TOKEN_EQUAL{"EQUAL"};
const std::string TOKEN_NEQ{"NEQ"};
const std::string TOKEN_LESS{"LESS"};
const std::string TOKEN_GREATER{"GREATER"};
const std::string TOKEN_LEQ{"LEQ"};
const std::string TOKEN_GEQ{"GEQ"};
// function
const std::string TOKEN_COMMA{"COMMA"};
const std::string TOKEN_COLON{"COLON"};
const std::string TOKEN_ARGS{"ARGS"};
// string
const std::string TOKEN_STRING{"STR"};
// Array
const std::string TOKEN_LEFT_BRACE{"LBRACE"};
const std::string TOKEN_RIGHT_BRACE{"RBRACE"};
const std::string TOKEN_LEFT_SQUARE{"LSQUARE"};
const std::string TOKEN_RIGHT_SQUARE{"RSQUARE"};
const std::string TOKEN_DOT{"DOT"};
// Error
const std::string TOKEN_ERROR{"ERROR"};

class Token {
public:
    Token(const std::string& _type = "", Position _pos = Position())
        : type(_type), pos(_pos) {}
    virtual std::string get_tok();
    virtual std::string get_type();
    virtual std::string get_value() { return "";}
    virtual Position get_pos() { return pos;}
    virtual inline bool isnumber() { return false;}
    friend std::ostream& operator<<(std::ostream &out, Token &token);
protected:
    std::string type;
    Position pos;
};

template<typename T>
class TypedToken: public Token {
public:
    TypedToken(const std::string& _type, const Position &_pos, const T &_value) 
        : Token(_type, _pos), value(_value) {}
    virtual std::string get_tok();
    virtual std::string get_value();
    virtual inline bool isnumber() { return type == TOKEN_INT || type == TOKEN_FLOAT;}
protected:
    T value;
};

using TokenList = std::vector<std::shared_ptr<Token>>;
using ErrorToken = TypedToken<std::string>;
using ArgsToken = TypedToken<TokenList>;
std::ostream& operator<<(std::ostream &out, TokenList &tokens);

/// --------------------
/// Node
/// --------------------

const std::string NODE_VALUE{"VALUE"};
const std::string NODE_BINOP{"BINOP"};
const std::string NODE_ERROR{"ERROR"};
const std::string NODE_UNARYOP("UNARYOP");
const std::string NODE_VARASSIGN("VARASSIGN");
const std::string NODE_VARACCESS("VARACCESS");
const std::string NODE_IF("IF");
const std::string NODE_FOR("FOR");
const std::string NODE_WHILE("WHILE");
const std::string NODE_REPEAT("REPEAT");
const std::string NODE_ALGODEF("ALGO");
const std::string NODE_ALGOCALL("CALL");
const std::string NODE_ARRAY("ARRAY");
const std::string NODE_ARRACCESS("ARRACCESS");
const std::string TAB{"    "};

class Node {
public:
    virtual std::string get_node() = 0;
    virtual ~Node() {};
    virtual std::vector<std::shared_ptr<Node>> get_child() { return std::vector<std::shared_ptr<Node>>(0);}
    virtual std::string get_type() {return "NONE";}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual TokenList get_toks() { return TokenList(0);}
    virtual std::string get_name() {return "";}
};

using NodeList = std::vector<std::shared_ptr<Node>>;

class ErrorNode: public Node {
public:
    ErrorNode(std::shared_ptr<Token> _tok)
        : tok(_tok) {}
    // virtual ~ErrorNode() {}
    virtual std::string get_node();
    virtual std::shared_ptr<Token> get_tok() { return tok;}
    virtual std::string get_type() {return NODE_ERROR;}
protected:
    std::shared_ptr<Token> tok;
};

class ValueNode: public Node {
public:
    ValueNode(std::shared_ptr<Token> _tok)
        : tok(_tok) {}
    // virtual ~ValueNode() {}
    virtual std::string get_node();
    virtual std::string get_type() {return NODE_VALUE;}
    virtual std::shared_ptr<Token> get_tok() { return tok;}
protected:
    std::shared_ptr<Token> tok;
};

class BinOpNode: public Node {
public:
    BinOpNode(std::shared_ptr<Node> left, std::shared_ptr<Node> right, std::shared_ptr<Token> tok)
        : left_node(left), right_node(right), op_tok(tok) {}
    // virtual ~BinOpNode() {}
    virtual std::string get_node();
    virtual NodeList get_child();
    virtual std::string get_type() {return NODE_BINOP;}
    virtual std::shared_ptr<Token> get_tok() { return op_tok;}
protected:
    std::shared_ptr<Node> left_node, right_node;
    std::shared_ptr<Token> op_tok;
};

class UnaryOpNode: public Node {
public:
    UnaryOpNode(std::shared_ptr<Node> _node, std::shared_ptr<Token> tok)
        : node(_node), op_tok(tok) {}
    // virtual ~UnaryOpNode() {}
    virtual std::string get_node();
    virtual NodeList get_child();
    virtual std::string get_type() { return NODE_UNARYOP;}
    virtual std::shared_ptr<Token> get_tok() { return op_tok;}
protected:
    std::shared_ptr<Node> node;
    std::shared_ptr<Token> op_tok;
};

class VarAssignNode: public Node {
public:
    VarAssignNode(std::string _name, std::shared_ptr<Node> _node)
        : name(_name), node(_node) {}
    // virtual ~VarAssignNode() {}
    virtual std::string get_node();
    virtual NodeList get_child() { return NodeList{node};}
    virtual std::string get_type() { return NODE_VARASSIGN;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual std::string get_name() {return name;}
protected:
    std::string name;
    std::shared_ptr<Node> node;
};

class VarAccessNode: public Node {
public:
    VarAccessNode(std::shared_ptr<Token> _tok)
        : tok(_tok) {}
    // virtual ~VarAccessNode() {}
    virtual std::string get_node();
    virtual NodeList get_child() { return NodeList(0);}
    virtual std::string get_type() { return NODE_VARACCESS;}
    virtual std::shared_ptr<Token> get_tok() { return tok;}
    virtual std::string get_name() {return tok->get_value();}
protected:
    std::shared_ptr<Token> tok;
};

class IfNode: public Node {
public:
    IfNode(std::shared_ptr<Node> condition, std::shared_ptr<Node> expr, std::shared_ptr<Node> _else_node)
        : condition_node(condition), expr_node(expr), else_node(_else_node) {}
    // virtual ~IfNode() {}
    virtual std::string get_node();
    virtual NodeList get_child() { return NodeList{condition_node, expr_node, else_node};}
    virtual std::string get_type() { return NODE_IF;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual std::string get_name() { return "";}
protected:
    std::shared_ptr<Node> condition_node, expr_node, else_node;
};

class ForNode: public Node {
public:
    ForNode(
        std::shared_ptr<Node> _var_assign, std::shared_ptr<Node> _end_value,
        std::shared_ptr<Node> _step_value, std::shared_ptr<Node> _body_node
    )   : var_assign(_var_assign), end_value(_end_value), step_value(_step_value), body_node(_body_node) {}
    virtual std::string get_node();
    virtual NodeList get_child() { return NodeList{var_assign, end_value, step_value, body_node};}
    virtual std::string get_type() { return NODE_FOR;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual std::string get_name() { return "";}
protected:
    std::shared_ptr<Node> var_assign, end_value, step_value, body_node;
};

class WhileNode: public Node {
public:
    WhileNode(std::shared_ptr<Node> _condition, std::shared_ptr<Node> _body_node)
        : condition(_condition), body_node(_body_node) {}
    virtual std::string get_node();
    virtual NodeList get_child() { return NodeList{condition, body_node};}
    virtual std::string get_type() { return NODE_WHILE;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual std::string get_name() { return "";}
protected:
    std::shared_ptr<Node> condition, body_node;
};

class RepeatNode: public Node {
public:
    RepeatNode(std::shared_ptr<Node> _body_node, std::shared_ptr<Node> _condition)
        : condition(_condition), body_node(_body_node) {}
    virtual std::string get_node();
    virtual NodeList get_child() { return NodeList{body_node,condition};}
    virtual std::string get_type() { return NODE_REPEAT;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual std::string get_name() { return "";}
protected:
    std::shared_ptr<Node> condition, body_node;
};

class AlgorithmDefNode: public Node {
public:
    AlgorithmDefNode(std::shared_ptr<Token> _algo_name, const TokenList &_args_name, NodeList _body_node = {})
        : algo_name(_algo_name), args_name(_args_name), body_node(_body_node) {}
    virtual std::string get_node();
    virtual NodeList get_child() { return body_node;}
    virtual std::string get_type() { return NODE_ALGODEF;}
    virtual std::shared_ptr<Token> get_tok() { return algo_name;}
    virtual TokenList get_toks() { return args_name;}
    virtual std::string get_name() { return algo_name->get_value();}
protected:
    std::shared_ptr<Token> algo_name;
    TokenList args_name;
    NodeList body_node;
};

class AlgorithmCallNode: public Node {
public:
    AlgorithmCallNode(std::shared_ptr<Node> _call_node, const NodeList &_args)
        : call_node(_call_node), args(_args) {}
    virtual std::string get_node();
    virtual NodeList get_child() { return args;}
    virtual std::string get_type() { return NODE_ALGOCALL;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual std::string get_name() { return call_node->get_name();}
protected:
    std::shared_ptr<Node> call_node;
    NodeList args;
};

class ArrayNode: public Node {
public:
    ArrayNode(const NodeList &_elements_node)
        : elements_node(_elements_node) {}
    virtual std::string get_node();
    virtual NodeList get_child() { return elements_node;}
    virtual std::string get_type() { return NODE_ARRAY;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual std::string get_name() { return "";}
protected:
    NodeList elements_node;
};

class ArrayAccessNode: public Node {
public:
    ArrayAccessNode(std::shared_ptr<Node> _arr, std::shared_ptr<Node> _index)
        : arr(_arr), index(_index) {}
    virtual std::string get_node();
    virtual NodeList get_child() { return NodeList{arr, index};}
    virtual std::string get_type() { return NODE_ARRACCESS;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
protected:
    std::shared_ptr<Node> arr, index;
};

/// --------------------
/// Value
/// --------------------

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
    std::shared_ptr<Value> operator[](int p);
    void push_back(std::shared_ptr<Value>);
    std::shared_ptr<Value> pop_back();
protected:
    ValueList value;
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


/// --------------------
/// Parser
/// --------------------

class Parser {
public:
    Parser(const TokenList& _tokens)
        : tokens(_tokens), tok_index(-1) { advance();}
    std::shared_ptr<Token> advance();
    std::shared_ptr<Node> factor();
    std::shared_ptr<Node> term();
    std::shared_ptr<Node> expr();
    std::shared_ptr<Node> arith_expr();
    std::shared_ptr<Node> comp_expr();
    std::shared_ptr<Node> array_expr();
    std::shared_ptr<Node> if_expr();
    std::shared_ptr<Node> for_expr();
    std::shared_ptr<Node> while_expr();
    std::shared_ptr<Node> repeat_expr();
    std::shared_ptr<Node> pow();
    std::shared_ptr<Node> atom();
    std::shared_ptr<Node> call();
    std::shared_ptr<Node> algo_def();
    
    std::shared_ptr<Node> bin_op(
        std::function<std::shared_ptr<Node>()>, 
        std::vector<std::string>, std::function<std::shared_ptr<Node>()>);
    std::shared_ptr<Node> parse();
protected:
    TokenList tokens;
    std::shared_ptr<Token> current_tok;
    int64_t tok_index;
};

/// --------------------
/// Lexer
/// --------------------

const std::map<char, std::string> TO_TOKEN_TYPE {
    {'+', TOKEN_ADD}, {'-', TOKEN_SUB}, 
    {'*', TOKEN_MUL}, {'/', TOKEN_DIV},
    {'%', TOKEN_MOD}, {'(', TOKEN_LEFT_PAREN},
    {')', TOKEN_RIGHT_PAREN}, {'^', TOKEN_POW},
    {'=', TOKEN_EQUAL}, {',', TOKEN_COMMA},
    {':', TOKEN_COLON}, {'{', TOKEN_LEFT_BRACE},
    {'}', TOKEN_RIGHT_BRACE}, {'[', TOKEN_LEFT_SQUARE},
    {']', TOKEN_RIGHT_SQUARE}
};

const std::set<std::string> KEYWORDS{
    "var", 
    "and", "or", "not",
    "for", "to", "step", "while", "do",
    "repeat", "until",
    "if", "then", "else", 
    "Algorithm", "continue", "break"
};

const std::map<std::string, int64_t> BUILTIN_CONST{
    {"true", 1}, {"false", 0}, {"none", 0}
};

const std::set<std::string> BUILTIN_ALGO{
    "print", "read", "read_line",
    "open", "clear", "quit",
    "int", "float", "string"
};

const std::map<char, char> ESCAPE_CHAR {
    {'n', '\n'}, {'r', '\r'},
    {'b', '\b'}, {'\"', '\"'},
    {'\'', '\''}, {'\\', '\\'},
    {'t', '\t'}, {'b', '\b'}
};

class Lexer {
public:
    Lexer(const std::string& _file_name, const std::string& _text)
        : file_name(_file_name), text(_text)
        , pos(-1, 0, -1, _file_name), current_char(NONE) {}
    void advance();
    TokenList make_tokens();
    std::shared_ptr<Token> make_number();
    std::shared_ptr<Token> make_identifier();
    std::shared_ptr<Token> make_string();
protected:
    std::string file_name, text;
    Position pos;
    char current_char;
};

/// --------------------
/// SymbolTable
/// --------------------

const std::map<std::string, std::shared_ptr<Value>> BUILTIN_ALGOS {
    {"print", std::make_shared<BuiltinAlgoValue>("print", 
        std::make_shared<AlgorithmDefNode>(std::make_shared<TypedToken<std::string>>(TOKEN_STRING, Position(), "print"), 
        TokenList{std::make_shared<TypedToken<std::string>>(TOKEN_STRING, Position(), "s")}))}, 
    {"read", std::make_shared<BuiltinAlgoValue>("read", 
        std::make_shared<AlgorithmDefNode>(std::make_shared<TypedToken<std::string>>(TOKEN_STRING, Position(), "read"), 
        TokenList{}))},
    {"read_line", std::make_shared<BuiltinAlgoValue>("read_line", 
        std::make_shared<AlgorithmDefNode>(std::make_shared<TypedToken<std::string>>(TOKEN_STRING, Position(), "read_line"), 
        TokenList{}))},
    {"open", std::make_shared<BuiltinAlgoValue>("open", 
        std::make_shared<AlgorithmDefNode>(std::make_shared<TypedToken<std::string>>(TOKEN_STRING, Position(), "open"), 
        TokenList{}))},
    {"clear", std::make_shared<BuiltinAlgoValue>("clear", 
        std::make_shared<AlgorithmDefNode>(std::make_shared<TypedToken<std::string>>(TOKEN_STRING, Position(), "clear"), 
        TokenList{}))},
    {"quit", std::make_shared<BuiltinAlgoValue>("quit", 
        std::make_shared<AlgorithmDefNode>(std::make_shared<TypedToken<std::string>>(TOKEN_STRING, Position(), "quit"), 
        TokenList{}))},
    {"int", std::make_shared<BuiltinAlgoValue>("int", 
        std::make_shared<AlgorithmDefNode>(std::make_shared<TypedToken<std::string>>(TOKEN_STRING, Position(), "int"), 
        TokenList{std::make_shared<TypedToken<std::string>>(TOKEN_STRING, Position(), "n")}))}, 
    {"float", std::make_shared<BuiltinAlgoValue>("float",
        std::make_shared<AlgorithmDefNode>(std::make_shared<TypedToken<std::string>>(TOKEN_STRING, Position(), "float"), 
        TokenList{std::make_shared<TypedToken<std::string>>(TOKEN_STRING, Position(), "n")}))}, 
    {"string", std::make_shared<BuiltinAlgoValue>("string", 
        std::make_shared<AlgorithmDefNode>(std::make_shared<TypedToken<std::string>>(TOKEN_STRING, Position(), "string"), 
        TokenList{std::make_shared<TypedToken<std::string>>(TOKEN_STRING, Position(), "s")}))}, 
};

class SymbolTable {
public:
    SymbolTable(SymbolTable *_parent = nullptr)
        : parent(_parent) {}
    std::shared_ptr<Value> get(std::string);
    void set(std::string, std::shared_ptr<Value>);
    void erase(std::string);
protected:
    std::map<std::string, std::shared_ptr<Value>> symbols;
    SymbolTable *parent;
};

/// --------------------
/// Interpreter
/// --------------------

class Interpreter {
public:
    Interpreter(SymbolTable &symbols)
        : symbol_table(symbols) {}
    std::shared_ptr<Value> visit(std::shared_ptr<Node>);
    std::shared_ptr<Value> visit_number(std::shared_ptr<Node>);
    std::shared_ptr<Value> visit_var_access(std::shared_ptr<Node>);
    std::shared_ptr<Value> visit_var_assign(std::shared_ptr<Node>);
    std::shared_ptr<Value> visit_bin_op(std::shared_ptr<Node>);
    std::shared_ptr<Value> visit_unary_op(std::shared_ptr<Node>);
    std::shared_ptr<Value> visit_array(std::shared_ptr<Node>);
    std::shared_ptr<Value> visit_if(std::shared_ptr<Node>);
    std::shared_ptr<Value> visit_for(std::shared_ptr<Node>);
    std::shared_ptr<Value> visit_while(std::shared_ptr<Node>);
    std::shared_ptr<Value> visit_repeat(std::shared_ptr<Node>);
    std::shared_ptr<Value> visit_algo_def(std::shared_ptr<Node>);
    std::shared_ptr<Value> visit_algo_call(std::shared_ptr<Node>);
    std::shared_ptr<Value> visit_array_access(std::shared_ptr<Node>);

    std::shared_ptr<Value> bin_op(std::shared_ptr<Value>, std::shared_ptr<Value>, std::shared_ptr<Token>);
    std::shared_ptr<Value> unary_op(std::shared_ptr<Value>, std::shared_ptr<Token>);
protected:
    SymbolTable &symbol_table;
};

/// --------------------
/// Run
/// --------------------

std::string Run(std::string, std::string, SymbolTable&);
#endif
