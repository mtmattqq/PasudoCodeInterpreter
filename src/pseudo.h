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

const std::string TOKEN_KEYWORD{"KEYWORD"};
const std::string TOKEN_IDENTIFIER{"IDENTIFIER"};
const std::string TOKEN_ASSIGN{"ASSIGN"};

const std::string TOKEN_INT{"INT"};
const std::string TOKEN_FLOAT{"FLOAT"};
// Arithmic
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
std::ostream& operator<<(std::ostream &out, TokenList &tokens);

/// --------------------
/// Number
/// --------------------

const std::string NUMBER_NONE{"NONE"};
const std::string NUMBER_INT{"Int"};
const std::string NUMBER_FLOAT{"Float"};
const std::string NUMBER_ERROR{"ERROR"};

class Number {
public:
    Number(const std::string& _type = NUMBER_NONE)
        : type(_type) {}
    virtual std::string get_num() { return "";}
    virtual std::string get_type(){ return type;}
    friend std::ostream& operator<<(std::ostream &out, Number &token);
    
protected:
    std::string type;
};

using NumberList = std::vector<std::shared_ptr<Number>>;

template<typename T>
class TypedNumber: public Number {
public:
    TypedNumber(const std::string& _type, const T &_value) 
        : Number(_type), value(_value) {}
    virtual std::string get_num();
protected:
    T value;
};

using ErrorNumber = TypedNumber<std::string>;

std::shared_ptr<Number> operator+(std::shared_ptr<Number>, std::shared_ptr<Number>);
std::shared_ptr<Number> operator-(std::shared_ptr<Number>, std::shared_ptr<Number>);
std::shared_ptr<Number> operator*(std::shared_ptr<Number>, std::shared_ptr<Number>);
std::shared_ptr<Number> operator/(std::shared_ptr<Number>, std::shared_ptr<Number>);
std::shared_ptr<Number> operator%(std::shared_ptr<Number>, std::shared_ptr<Number>);
std::shared_ptr<Number> pow(std::shared_ptr<Number>, std::shared_ptr<Number>);

std::shared_ptr<Number> operator==(std::shared_ptr<Number>, std::shared_ptr<Number>);
std::shared_ptr<Number> operator!=(std::shared_ptr<Number>, std::shared_ptr<Number>);
std::shared_ptr<Number> operator<(std::shared_ptr<Number>, std::shared_ptr<Number>);
std::shared_ptr<Number> operator>(std::shared_ptr<Number>, std::shared_ptr<Number>);
std::shared_ptr<Number> operator<=(std::shared_ptr<Number>, std::shared_ptr<Number>);
std::shared_ptr<Number> operator>=(std::shared_ptr<Number>, std::shared_ptr<Number>);

std::shared_ptr<Number> operator&&(std::shared_ptr<Number>, std::shared_ptr<Number>);
std::shared_ptr<Number> operator||(std::shared_ptr<Number>, std::shared_ptr<Number>);

std::shared_ptr<Number> operator-(std::shared_ptr<Number>);
std::shared_ptr<Number> operator!(std::shared_ptr<Number>);

/// --------------------
/// Node
/// --------------------

const std::string NODE_NUMBER{"NUMBER"};
const std::string NODE_BINOP{"BINOP"};
const std::string NODE_ERROR{"ERROR"};
const std::string NODE_UNARYOP("UNARYOP");
const std::string NODE_VARASSIGN("VARASSIGN");
const std::string NODE_VARACCESS("VARACCESS");
const std::string NODE_IF("IF");
const std::string NODE_FOR("FOR");
const std::string NODE_WHILE("WHILE");
const std::string NODE_REPEAT("REPEAT");

class Node {
public:
    virtual std::string get_node() = 0;
    virtual ~Node() {};
    virtual std::vector<std::shared_ptr<Node>> get_child() { return std::vector<std::shared_ptr<Node>>(0);}
    virtual std::string get_type() {return "";}
    virtual std::shared_ptr<Token> get_tok() = 0;
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

class NumberNode: public Node {
public:
    NumberNode(std::shared_ptr<Token> _tok)
        : tok(_tok) {}
    // virtual ~NumberNode() {}
    virtual std::string get_node();
    virtual std::string get_type() {return NODE_NUMBER;}
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
    RepeatNode( std::shared_ptr<Node> _body_node, std::shared_ptr<Node> _condition)
        : condition(_condition), body_node(_body_node) {}
    virtual std::string get_node();
    virtual NodeList get_child() { return NodeList{body_node,condition};}
    virtual std::string get_type() { return NODE_REPEAT;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual std::string get_name() { return "";}
protected:
    std::shared_ptr<Node> condition, body_node;
};

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
    std::shared_ptr<Node> if_expr();
    std::shared_ptr<Node> for_expr();
    std::shared_ptr<Node> while_expr();
    std::shared_ptr<Node> repeat_expr();

    std::shared_ptr<Node> pow();
    std::shared_ptr<Node> atom();
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

const std::set<std::string> KEYWORDS{
    "var", 
    "and", "or", "not",
    "for", "to", "step", "while", "do",
    "repeat", "until",
    "if", "then", "else", 
    "Algotithm"
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
protected:
    std::string file_name, text;
    Position pos;
    char current_char;
};

/// --------------------
/// SymbolTable
/// --------------------

class SymbolTable {
public:
    SymbolTable()
        : parent(nullptr) {}
    std::shared_ptr<Number> get(std::string);
    void set(std::string, std::shared_ptr<Number>);
    void erase(std::string);
protected:
    std::map<std::string, std::shared_ptr<Number>> symbols;
    std::shared_ptr<SymbolTable> parent;
};

/// --------------------
/// Interpreter
/// --------------------

class Interpreter {
public:
    Interpreter(SymbolTable &symbols)
        : symbol_table(symbols) {}
    std::shared_ptr<Number> visit(std::shared_ptr<Node>);
    std::shared_ptr<Number> visit_number(std::shared_ptr<Node>);
    std::shared_ptr<Number> visit_var_access(std::shared_ptr<Node>);
    std::shared_ptr<Number> visit_var_assign(std::shared_ptr<Node>);
    std::shared_ptr<Number> visit_bin_op(std::shared_ptr<Node>);
    std::shared_ptr<Number> visit_unary_op(std::shared_ptr<Node>);
    std::shared_ptr<Number> visit_if(std::shared_ptr<Node>);
    std::shared_ptr<Number> visit_for(std::shared_ptr<Node>);
    std::shared_ptr<Number> visit_while(std::shared_ptr<Node>);
    std::shared_ptr<Number> visit_repeat(std::shared_ptr<Node>);

    std::shared_ptr<Number> bin_op(std::shared_ptr<Number>, std::shared_ptr<Number>, std::shared_ptr<Token>);
    std::shared_ptr<Number> unary_op(std::shared_ptr<Number>, std::shared_ptr<Token>);
protected:
    SymbolTable &symbol_table;
};

/// --------------------
/// Run
/// --------------------

std::string Run(std::string, std::string, SymbolTable&);
#endif