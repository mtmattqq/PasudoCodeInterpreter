#include "pseudo.h"
#include <iostream>
#include <sstream>

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
    ss << type << ": " << value;
    std::string ret;
    std::getline(ss, ret);
    return ret;
}

std::ostream& operator<<(std::ostream &out, TokenList &tokens) {
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

BinOpNode::~BinOpNode() {
    left_node.reset();
    right_node.reset();
    op_tok.reset();
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
    if(tok->isnumber()) {
        advance();
        return std::make_shared<NumberNode>(tok);
    }
    return std::make_shared<ErrorNode>(std::make_shared<ErrorToken>(TOKEN_ERROR, "Not a factor"));
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
    return expr();
}

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
            
            case '+':
            tokens.push_back(std::make_shared<Token>(TOKEN_ADD));
            advance(); break;
            
            case '-':
            tokens.push_back(std::make_shared<Token>(TOKEN_SUB));
            advance(); break;
            
            case '*':
            tokens.push_back(std::make_shared<Token>(TOKEN_MUL));
            advance(); break;
          
            case '/':
            tokens.push_back(std::make_shared<Token>(TOKEN_DIV));
            advance(); break;
          
            case '%':
            tokens.push_back(std::make_shared<Token>(TOKEN_MOD));
            advance(); break;
            
            case '(':
            tokens.push_back(std::make_shared<Token>(TOKEN_LEFT_PAREN));
            advance(); break;
            
            case ')':
            tokens.push_back(std::make_shared<Token>(TOKEN_RIGHT_PAREN));
            advance(); break;

            default:
            tokens.clear();
            std::string error_msg = "Illegal char \'";
            error_msg += current_char;
            error_msg += "\'. At " + pos.get_pos();
            tokens.push_back(std::make_shared<ErrorToken>(TOKEN_ERROR, error_msg));
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
        return std::make_shared<TypedToken<int64_t>>(TOKEN_INT, std::stoll(number_str));
    else 
        return std::make_shared<TypedToken<double>>(TOKEN_FLOAT, std::stod(number_str));
}

/// --------------------
/// Run
/// --------------------

std::string Run(std::string file_name, std::string text) {
    Lexer lexer(file_name, text);
    TokenList tokens = lexer.make_tokens();
    std::cout << tokens << "\n";
    Parser parser(tokens);
    std::shared_ptr<Node> ast = parser.parse();
    std::cout << ast->get_node() << "\n";
    return "";
}