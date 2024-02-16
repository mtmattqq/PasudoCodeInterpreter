#include "pseudo.h"
#include "color.h"
#include <iostream>
#include <sstream>

/// --------------------
/// Position
/// --------------------

void Position::advance(char current_char) {
    index++;
    column++;
    if(current_char == '\n') {
        line++;
        column = 0;
    }
}

std::string Position::get_pos() {
    std::stringstream ss;
    ss << (*this);
    std::string ret;
    std::getline(ss, ret);
    return ret;
}

std::ostream& operator<<(std::ostream &out, Position &pos) {
    out << "File: " << pos.file_name << ", Line: " << pos.line << ", Column: " << pos.column;
    return out;
}

/// --------------------
/// Token
/// --------------------

std::string Token::get_tok() {
    return type;
}

std::string Token::get_type() {
    return type;
}

std::ostream& operator<<(std::ostream &out, Token &token) {
    out << token.get_tok();
    return out;
}

template<typename T>
std::string TypedToken<T>::get_tok() {
    std::stringstream ss;
    if(type == TOKEN_ERROR)
        ss << Color(0xFF, 0x39, 0x6E);
    ss << type << ": " << value;
    if(type == TOKEN_ERROR)
        ss << Color(0xDB, 0x80, 0xFF) << ". At " << pos << RESET;
    std::string ret;
    std::getline(ss, ret);
    return ret;
}

std::ostream& operator<<(std::ostream &out, TokenList &tokens) {
    if(!tokens.empty() && tokens.front()->get_type() == TOKEN_ERROR) {
        out << (*tokens.front());
        return out;
    }
    
    out << "{";
    for(auto tok : tokens) {
        out << (*tok);
        if(tok != tokens.back())
            out << ", ";
    }
    out << "}";
    return out;
}

/// --------------------
/// Number
/// --------------------

std::ostream& operator<<(std::ostream &out, Number &number) {
    out << number.get_num();
    return out;
}

template<typename T>
std::string TypedNumber<T>::get_num() {
    std::stringstream ss;
    ss << value;
    std::string ret;
    std::getline(ss, ret);
    return ret;
}

template<typename T>
T TypedNumber<T>::get_value() {
    return value;
}

/// --------------------
/// Node
/// --------------------

std::string NumberNode::get_node() {
    return tok->get_tok();
}

std::string ErrorNode::get_node() {
    return tok->get_tok();
}

std::string BinOpNode::get_node() {
    std::stringstream ss;
    ss << "(" << left_node->get_node() << ", " << op_tok->get_tok() << ", " << right_node->get_node() << ")";
    std::string ret;
    std::getline(ss, ret);
    return ret;
}

NodeList BinOpNode::get_child() {
    return NodeList{left_node, right_node};
}

BinOpNode::~BinOpNode() {
    left_node.reset();
    right_node.reset();
    op_tok.reset();
}

std::string UnaryOpNode::get_node() {
    std::stringstream ss;
    ss << "(" << op_tok->get_tok() << ", " << node->get_node() << ")";
    std::string ret;
    std::getline(ss, ret);
    return ret;
}

NodeList UnaryOpNode::get_child() {
    return NodeList{node};
}

/// --------------------
/// Parser
/// --------------------

std::shared_ptr<Token> Parser::advance() {
    tok_index++;
    if(tok_index < tokens.size())
        current_tok = tokens[tok_index];
    else
        current_tok = std::make_shared<Token>();
    return current_tok;
}

std::shared_ptr<Node> Parser::factor() {
    std::shared_ptr<Token> tok = current_tok;
    std::string error_msg{"Not a factor: Expect an INT or a FLOAT, found \""};
    if(tok->get_type() == TOKEN_ADD || tok->get_type() == TOKEN_SUB) {
        advance();
        return std::make_shared<UnaryOpNode>(factor(), tok);
    } else if(tok->isnumber()) {
        advance();
        return std::make_shared<NumberNode>(tok);
    } else if(tok->get_type() == TOKEN_LEFT_PAREN) {
        advance();
        std::shared_ptr<Node> e = expr();
        if(current_tok->get_type() == TOKEN_RIGHT_PAREN) {
            advance();
            return e;
        }
        error_msg = "Not a factor: Expect \')\', found \"";
    }
    error_msg += current_tok->get_tok() + "\"";
    std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
    return std::make_shared<ErrorNode>(error_token);
}

std::shared_ptr<Node> Parser::term() {
    return bin_op(std::bind(&Parser::factor, this), {TOKEN_MUL, TOKEN_DIV});
}

std::shared_ptr<Node> Parser::expr() {
    return bin_op(std::bind(&Parser::term, this), {TOKEN_ADD, TOKEN_SUB});
}

std::shared_ptr<Node> Parser::bin_op(std::function<std::shared_ptr<Node>()> func, std::vector<std::string> allowed_types) {
    std::shared_ptr<Node> left = func();
    if(left->get_type() == NODE_ERROR) 
        return left;
    while(std::find(allowed_types.begin(), allowed_types.end(), current_tok->get_type()) != allowed_types.end()) {
        std::shared_ptr<Token> op_tok = current_tok;
        advance();
        std::shared_ptr<Node> right = func();
        if(right->get_type() == NODE_ERROR) 
            return right;
        left = std::make_shared<BinOpNode>(left, right, op_tok);
    }
    return left;
}

std::shared_ptr<Node> Parser::parse() {
    std::shared_ptr<Node> ret = expr();
    if(ret->get_type() == NODE_ERROR) {
        return ret;
    }
    if(tok_index != tokens.size()) {
        std::string error_msg = "Syntex Error: Found resundant tokens: ";
        Position pos = tokens[tok_index]->get_pos();
        while(tok_index != tokens.size())
            error_msg += tokens[tok_index++]->get_tok();
            if(tok_index != tokens.size())
                error_msg += ", ";
        return std::make_shared<ErrorNode>(std::make_shared<ErrorToken>(TOKEN_ERROR, pos, error_msg));
    }
    return ret;
}

/// --------------------
/// Lexer
/// --------------------

void Lexer::advance() {
    pos.advance(current_char);
    if(pos.index >= text.size()) {
        current_char = NONE;
        return;
    }
    current_char = text[pos.index];
}

TokenList Lexer::make_tokens() {
    static std::map<char, std::string> TO_TOKEN_TYPE {
        {'+', TOKEN_ADD}, {'-', TOKEN_SUB}, 
        {'*', TOKEN_MUL}, {'/', TOKEN_DIV},
        {'%', TOKEN_MOD}, {'(', TOKEN_LEFT_PAREN},
        {')', TOKEN_RIGHT_PAREN}
    };

    TokenList tokens;
    advance();
    while(current_char != NONE) {
        if(std::isdigit(current_char)) {
            tokens.push_back(make_number());
            continue;
        }
        switch(current_char) {
            case ' ': case '\t': case '\n':
            advance(); break;
            
            case '+': case '-': case '*': case '/': case '%': case '(': case ')':
            tokens.push_back(std::make_shared<Token>(TO_TOKEN_TYPE[current_char], pos));
            advance(); break;

            default:
            tokens.clear();
            std::string error_msg = "Illegal char \'";
            error_msg += current_char;
            error_msg += "\'. At " + pos.get_pos();
            tokens.push_back(std::make_shared<ErrorToken>(TOKEN_ERROR, pos, error_msg));
            return tokens;
        }
    }
    return tokens;
}

std::shared_ptr<Token> Lexer::make_number() {
    std::string number_str;
    int dot_count{0};

    while(current_char != NONE && (std::isdigit(current_char) || current_char == '.')) {
        if(current_char == '.') {
            if(dot_count == 1) break;
            dot_count++;
        }
        number_str += current_char;
        advance();
    }

    if(dot_count == 0) 
        return std::make_shared<TypedToken<int64_t>>(TOKEN_INT, pos, std::stoll(number_str));
    else 
        return std::make_shared<TypedToken<double>>(TOKEN_FLOAT, pos, std::stod(number_str));
}

/// --------------------
/// Interpreter
/// --------------------

void Interpreter::visit(std::shared_ptr<Node> node) {
    if(node->get_type() == NODE_NUMBER) {
        std::cerr << "Found a number" << "\n";
    }
    NodeList child = node->get_child();
    if(node->get_type() == NODE_BINOP) {
        std::cerr << "Found a BinOp" << "\n";
    }
    if(node->get_type() == NODE_UNARYOP) {
        std::cerr << "Found a UnaryOp" << "\n";
    }
    for(auto c : child) {
        visit(c);
    }
}

/// --------------------
/// Run
/// --------------------

std::string Run(std::string file_name, std::string text) {
    Lexer lexer(file_name, text);
    TokenList tokens = lexer.make_tokens();
    if(tokens.empty()) return "";
    std::cout << "Tokens: " << tokens << "\n";

    Parser parser(tokens);
    std::shared_ptr<Node> ast = parser.parse();
    std::cout << "Nodes: " << ast->get_node() << "\n";
    if(ast->get_type() == NODE_ERROR) return "STOP";

    Interpreter interpreter;
    interpreter.visit(ast);
    return "";
}