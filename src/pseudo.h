#ifndef PSEUDO_H
#define PSEUDO_H

#include <vector>
#include <string>
#include <memory>
#include <functional>

#define NONE 0

/// --------------------
/// Token
/// --------------------

const std::string TOKEN_INT{"INT"};
const std::string TOKEN_FLOAT{"FLOAT"};
const std::string TOKEN_ADD{"ADD"};
const std::string TOKEN_SUB{"SUB"};
const std::string TOKEN_MUL{"MUL"};
const std::string TOKEN_DIV{"DIV"};
const std::string TOKEN_MOD{"MOD"};
const std::string TOKEN_LEFT_PAREN{"LPAREN"};
const std::string TOKEN_RIGHT_PAREN{"RPAREN"};
const std::string TOKEN_ERROR{"ERROR"};

class Token {
public:
    Token(const std::string& _type = "")
        : type(_type) {}
    virtual std::string get_tok();
    virtual std::string get_type();
    virtual inline bool isnumber() { return false;}
    friend std::ostream& operator<<(std::ostream &out, Token &token);
protected:
    std::string type;
};

template<typename T>
class TypedToken: public Token {
public:
    TypedToken(const std::string& _type, const T &_value) 
        : Token(_type), value(_value) {}
    virtual std::string get_tok();
    virtual inline bool isnumber() { return type == TOKEN_INT || type == TOKEN_FLOAT;}
protected:
    T value;
};

using TokenList = std::vector<std::shared_ptr<Token>>;
using ErrorToken = TypedToken<std::string>;
std::ostream& operator<<(std::ostream &out, TokenList &tokens);

/// --------------------
/// Node
/// --------------------

const std::string NODE_NUMBER{"NUMBER"};
const std::string NODE_BINOP{"BINOP"};
const std::string NODE_ERROR{"ERROR"};


class Node {
public:
    virtual std::string get_node() = 0;
    virtual ~Node() {};
    virtual std::string get_type() {return "";}
};

class ErrorNode: public Node {
public:
    ErrorNode(std::shared_ptr<ErrorToken> _tok)
        : tok(_tok) {}
    virtual std::string get_node();
    virtual ~ErrorNode() {tok.reset();}
    virtual std::string get_type() {return NODE_ERROR;}
protected:
    std::shared_ptr<ErrorToken> tok;
};

class NumberNode: public Node {
public:
    NumberNode(std::shared_ptr<Token> _tok)
        : tok(_tok) {}
    virtual std::string get_node();
    virtual ~NumberNode() {tok.reset();}
    virtual std::string get_type() {return NODE_NUMBER;}
protected:
    std::shared_ptr<Token> tok;
};

class BinOpNode: public Node {
public:
    BinOpNode(std::shared_ptr<Node> left, std::shared_ptr<Node> right, std::shared_ptr<Token> tok)
        : left_node(left), right_node(right), op_tok(tok) {}
    virtual std::string get_node();
    virtual ~BinOpNode();
    virtual std::string get_type() {return NODE_BINOP;}
protected:
    std::shared_ptr<Node> left_node, right_node;
    std::shared_ptr<Token> op_tok;
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
    std::shared_ptr<Node> bin_op(std::function<std::shared_ptr<Node>()>, std::vector<std::string>);
    std::shared_ptr<Node> parse();
protected:
    TokenList tokens;
    std::shared_ptr<Token> current_tok;
    int64_t tok_index;
};

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
/// Lexer
/// --------------------

class Lexer {
public:
    Lexer(const std::string& _file_name, const std::string& _text)
        : file_name(_file_name), text(_text)
        , pos(-1, 0, -1, _file_name), current_char(NONE) {}
    void advance();
    TokenList make_tokens();
    std::shared_ptr<Token> make_number();
protected:
    std::string file_name, text;
    Position pos;
    char current_char;
};

std::string Run(std::string, std::string);


#endif