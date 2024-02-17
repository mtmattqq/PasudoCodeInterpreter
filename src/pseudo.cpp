#include "pseudo.h"
#include "color.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

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
        ss << Color(0xDB, 0x80, 0xFF) << ". At " << pos << RESET "\n";
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

template<typename T>
std::string TypedToken<T>::get_value() {
    std::stringstream ss;
    ss << value;
    std::string ret;
    std::getline(ss, ret);
    return ret;
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


std::shared_ptr<Number> operator+(std::shared_ptr<Number> a, std::shared_ptr<Number> b) {
    if(a->get_type() == NUMBER_FLOAT || b->get_type() == NUMBER_FLOAT)
        return std::make_shared<TypedNumber<double>>(
            NUMBER_FLOAT, std::stod(a->get_num()) + std::stod(b->get_num()));
    else
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stoll(a->get_num()) + std::stoll(b->get_num()));
}

std::shared_ptr<Number> operator-(std::shared_ptr<Number> a, std::shared_ptr<Number> b) {
    if(a->get_type() == NUMBER_FLOAT || b->get_type() == NUMBER_FLOAT)
        return std::make_shared<TypedNumber<double>>(
            NUMBER_FLOAT, std::stod(a->get_num()) - std::stod(b->get_num()));
    else
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stoll(a->get_num()) - std::stoll(b->get_num()));
}

std::shared_ptr<Number> operator*(std::shared_ptr<Number> a, std::shared_ptr<Number> b) {
    if(a->get_type() == NUMBER_FLOAT || b->get_type() == NUMBER_FLOAT)
        return std::make_shared<TypedNumber<double>>(
            NUMBER_FLOAT, std::stod(a->get_num()) * std::stod(b->get_num()));
    else
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stoll(a->get_num()) * std::stoll(b->get_num()));
}

std::shared_ptr<Number> operator/(std::shared_ptr<Number> a, std::shared_ptr<Number> b) {
    if(std::stod(b->get_num()) == 0.0) 
        return std::make_shared<ErrorNumber>(NUMBER_ERROR, "Runtime ERROR: DIV by 0\n");
    if(a->get_type() == NUMBER_FLOAT || b->get_type() == NUMBER_FLOAT)
        return std::make_shared<TypedNumber<double>>(
            NUMBER_FLOAT, std::stod(a->get_num()) / std::stod(b->get_num()));
    else
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stoll(a->get_num()) / std::stoll(b->get_num()));
}

std::shared_ptr<Number> operator%(std::shared_ptr<Number> a, std::shared_ptr<Number> b) {
    if(a->get_type() != NUMBER_INT || b->get_type() != NUMBER_INT) 
        return std::make_shared<ErrorNumber>(NUMBER_ERROR, "Float doesn't have \"%\" operation\n");
    return std::make_shared<TypedNumber<int64_t>>(
        NUMBER_INT, std::stoll(a->get_num()) % std::stoll(b->get_num()));
}

std::shared_ptr<Number> operator==(std::shared_ptr<Number> a, std::shared_ptr<Number> b) {
    if(a->get_type() == NUMBER_FLOAT || b->get_type() == NUMBER_FLOAT)
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stod(a->get_num()) == std::stod(b->get_num()));
    else
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stoll(a->get_num()) == std::stoll(b->get_num()));
}

std::shared_ptr<Number> operator!=(std::shared_ptr<Number> a, std::shared_ptr<Number> b) {
    if(a->get_type() == NUMBER_FLOAT || b->get_type() == NUMBER_FLOAT)
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stod(a->get_num()) != std::stod(b->get_num()));
    else
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stoll(a->get_num()) != std::stoll(b->get_num()));
}

std::shared_ptr<Number> operator<(std::shared_ptr<Number> a, std::shared_ptr<Number> b) {
    if(a->get_type() == NUMBER_FLOAT || b->get_type() == NUMBER_FLOAT)
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stod(a->get_num()) < std::stod(b->get_num()));
    else
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stoll(a->get_num()) < std::stoll(b->get_num()));
}

std::shared_ptr<Number> operator>(std::shared_ptr<Number> a, std::shared_ptr<Number> b) {
    if(a->get_type() == NUMBER_FLOAT || b->get_type() == NUMBER_FLOAT)
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stod(a->get_num()) > std::stod(b->get_num()));
    else
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stoll(a->get_num()) > std::stoll(b->get_num()));
}

std::shared_ptr<Number> operator<=(std::shared_ptr<Number> a, std::shared_ptr<Number> b) {
    if(a->get_type() == NUMBER_FLOAT || b->get_type() == NUMBER_FLOAT)
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stod(a->get_num()) <= std::stod(b->get_num()));
    else
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stoll(a->get_num()) <= std::stoll(b->get_num()));
}

std::shared_ptr<Number> operator>=(std::shared_ptr<Number> a, std::shared_ptr<Number> b) {
    if(a->get_type() == NUMBER_FLOAT || b->get_type() == NUMBER_FLOAT)
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stod(a->get_num()) >= std::stod(b->get_num()));
    else
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stoll(a->get_num()) >= std::stoll(b->get_num()));
}

std::shared_ptr<Number> operator&&(std::shared_ptr<Number> a, std::shared_ptr<Number> b) {
    if(a->get_type() == NUMBER_FLOAT || b->get_type() == NUMBER_FLOAT)
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stod(a->get_num()) != 0 && std::stod(b->get_num()) != 0);
    else
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stoll(a->get_num()) && std::stoll(b->get_num()));
}

std::shared_ptr<Number> operator||(std::shared_ptr<Number> a, std::shared_ptr<Number> b) {
    if(a->get_type() == NUMBER_FLOAT || b->get_type() == NUMBER_FLOAT)
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stod(a->get_num()) != 0 || std::stod(b->get_num()) != 0);
    else
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::stoll(a->get_num()) || std::stoll(b->get_num()));
}

std::shared_ptr<Number> operator-(std::shared_ptr<Number> a) {
    if(a->get_type() == NUMBER_FLOAT)
        return std::make_shared<TypedNumber<double>>(NUMBER_FLOAT, 0 - stod(a->get_num()));
    else
        return std::make_shared<TypedNumber<int64_t>>(NUMBER_INT, 0 - stoll(a->get_num()));
}

std::shared_ptr<Number> operator!(std::shared_ptr<Number> a) {
    if(a->get_type() == NUMBER_FLOAT)
        return std::make_shared<TypedNumber<double>>(NUMBER_FLOAT, stod(a->get_num()) == 0);
    else
        return std::make_shared<TypedNumber<int64_t>>(NUMBER_INT, stoll(a->get_num()) == 0);
}

std::shared_ptr<Number> pow(std::shared_ptr<Number> a, std::shared_ptr<Number> b) {
    if(std::stod(a->get_num()) == 0.0 && std::stod(b->get_num()) == 0.0) 
        return std::make_shared<ErrorNumber>(NUMBER_ERROR, "Runtime ERROR: 0 to the 0\n");
    if(a->get_type() == NUMBER_FLOAT || b->get_type() == NUMBER_FLOAT)
        return std::make_shared<TypedNumber<double>>(
            NUMBER_FLOAT, std::pow(std::stod(a->get_num()), std::stod(b->get_num())));
    else
        return std::make_shared<TypedNumber<int64_t>>(
            NUMBER_INT, std::pow(std::stoll(a->get_num()), std::stoll(b->get_num())));
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

std::string VarAssignNode::get_node() {
    std::stringstream ss;
    ss << "(VAR " << name << " <- " << node->get_node() << ")";
    std::string ret;
    std::getline(ss, ret);
    return ret;
}

std::string VarAccessNode::get_node() {
    return "(ACCESS: " + tok->get_tok() + ")";
}

std::string IfNode::get_node() {
    std::stringstream ss;
    ss << "(IF " << condition_node->get_node() << " THEN " << expr_node->get_node();
    if(else_node != nullptr)
        ss << " ELSE " << else_node->get_node();
    ss << ")";
    std::string ret;
    std::getline(ss, ret);
    return ret;
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

std::shared_ptr<Node> Parser::atom() {
    std::shared_ptr<Token> tok = current_tok;
    std::string error_msg{"Not a atom, found \""};
    if(tok->isnumber()) {
        advance();
        return std::make_shared<NumberNode>(tok);
    } else if(tok->get_type() == TOKEN_LEFT_PAREN) {
        advance();
        std::shared_ptr<Node> e = expr();
        if(current_tok->get_type() == TOKEN_RIGHT_PAREN) {
            advance();
            return e;
        }
    } else if(tok->get_type() == TOKEN_IDENTIFIER) {
        advance();
        return std::make_shared<VarAccessNode>(tok);
    } else if(tok->get_type() == TOKEN_KEYWORD && tok->get_value() == "if") {
        advance();
        return if_expr();
    }
    error_msg += current_tok->get_tok() + "\"\n";
    std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
    return std::make_shared<ErrorNode>(error_token);
}

std::shared_ptr<Node> Parser::factor() {
    std::shared_ptr<Token> tok = current_tok;
    if(tok->get_type() == TOKEN_ADD || tok->get_type() == TOKEN_SUB) {
        advance();
        return std::make_shared<UnaryOpNode>(factor(), tok);
    }
    return pow();
}

std::shared_ptr<Node> Parser::term() {
    return bin_op(std::bind(&Parser::factor, this), {TOKEN_MUL, TOKEN_DIV}, std::bind(&Parser::factor, this));
}

std::shared_ptr<Node> Parser::expr() {
    if(current_tok->get_type() == TOKEN_KEYWORD && current_tok->get_value() == "var") {
        advance();
        if(current_tok->get_type() != TOKEN_IDENTIFIER) {
            std::string error_msg = "Expected Identifier\n";
            std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
            return std::make_shared<ErrorNode>(error_token);
        }
        std::string var_name = current_tok->get_value();
        advance();
        if(current_tok->get_type() != TOKEN_ASSIGN) {
            std::string error_msg = "Expected \"<-\"\n";
            std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
            return std::make_shared<ErrorNode>(error_token);
        }
        advance();
        std::shared_ptr<Node> ret = expr();
        if(ret->get_type() == NODE_ERROR) return ret;
        return std::make_shared<VarAssignNode>(var_name, ret);
    }
    return bin_op(std::bind(&Parser::comp_expr, this), {"and", "or"}, std::bind(&Parser::comp_expr, this));
}

std::shared_ptr<Node> Parser::comp_expr() {
    std::shared_ptr<Token> tok = current_tok;
    if(current_tok->get_type() == TOKEN_KEYWORD && current_tok->get_value() == "not") {
        advance();
        std::shared_ptr<Node> ret = comp_expr();
        if(ret->get_type() == NODE_ERROR) return ret;
        return std::make_shared<UnaryOpNode>(ret, tok);
    }
    return bin_op(std::bind(&Parser::arith_expr, this), 
        {TOKEN_EQUAL, TOKEN_NEQ, TOKEN_LESS, TOKEN_GREATER, TOKEN_LEQ, TOKEN_GEQ}, std::bind(&Parser::arith_expr, this));
}

std::shared_ptr<Node> Parser::arith_expr() {
    return bin_op(std::bind(&Parser::term, this), {TOKEN_ADD, TOKEN_SUB}, std::bind(&Parser::term, this));
}

std::shared_ptr<Node> Parser::if_expr() {
    std::shared_ptr<Node> condtion = expr();
    if(!(current_tok->get_type() == TOKEN_KEYWORD && current_tok->get_value() == "then")) {
        std::string error_msg = "Expected \"then\"\n";
        std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
        return std::make_shared<ErrorNode>(error_token);
    }
    advance();
    std::shared_ptr<Node> exp = expr();
    std::shared_ptr<Node> els = nullptr;
    if(current_tok->get_type() == TOKEN_KEYWORD && current_tok->get_value() == "else") {
        advance();
        if(current_tok->get_type() == TOKEN_KEYWORD && current_tok->get_value() == "if") {
            advance();
            els = if_expr();
        } else {
            els = expr();
        }
    }
    return std::make_shared<IfNode>(condtion, exp, els);
}

std::shared_ptr<Node> Parser::pow() {
    return bin_op(std::bind(&Parser::atom, this), {TOKEN_POW}, std::bind(&Parser::factor, this));
}

std::shared_ptr<Node> Parser::bin_op(
    std::function<std::shared_ptr<Node>()> lfunc, 
    std::vector<std::string> allowed_types, 
    std::function<std::shared_ptr<Node>()> rfunc
) {
    std::shared_ptr<Node> left = lfunc();
    if(left->get_type() == NODE_ERROR) 
        return left;
    while(
        std::find(allowed_types.begin(), allowed_types.end(), current_tok->get_type()) != allowed_types.end() ||
        std::find(allowed_types.begin(), allowed_types.end(), current_tok->get_value()) != allowed_types.end()
    ) {
        std::shared_ptr<Token> op_tok = current_tok;
        advance();
        std::shared_ptr<Node> right = rfunc();
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
    // if(tok_index != tokens.size()) {
    //     std::string error_msg = "Syntex Error: Found resundant tokens: ";
    //     Position pos = tokens[tok_index]->get_pos();
    //     while(tok_index != tokens.size())
    //         error_msg += tokens[tok_index++]->get_tok();
    //         if(tok_index != tokens.size())
    //             error_msg += ", ";
    //     error_msg += "\n";
    //     return std::make_shared<ErrorNode>(std::make_shared<ErrorToken>(TOKEN_ERROR, pos, error_msg));
    // }
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
        {')', TOKEN_RIGHT_PAREN}, {'^', TOKEN_POW},
        {'=', TOKEN_EQUAL}
    };

    TokenList tokens;
    advance();
    while(current_char != NONE) {
        if(std::isdigit(current_char)) {
            tokens.push_back(make_number());
            continue;
        }
        if(std::isalpha(current_char)) {
            tokens.push_back(make_identifier());
            continue;
        }

        switch(current_char) {
            case ' ': case '\t': case '\n':
            advance(); break;
            
            case '+': case '-': case '*': case '/': case '%': case '^': case '(': case ')':
            case '=':
            tokens.push_back(std::make_shared<Token>(TO_TOKEN_TYPE[current_char], pos));
            advance(); break;
            case '<':
            advance();
            if(current_char == '-') {
                tokens.push_back(std::make_shared<Token>(TOKEN_ASSIGN, pos));
                advance();
            } else if(current_char == '=') {
                tokens.push_back(std::make_shared<Token>(TOKEN_LEQ, pos));
                advance();
            } else {
                tokens.push_back(std::make_shared<Token>(TOKEN_LESS, pos));
            } break;

            case '>':
            advance();
            if(current_char == '=') {
                tokens.push_back(std::make_shared<Token>(TOKEN_GEQ, pos));
                advance();
            } else {
                tokens.push_back(std::make_shared<Token>(TOKEN_GREATER, pos));
            } break;

            case '!':
            advance();
            if(current_char == '=') {
                tokens.push_back(std::make_shared<Token>(TOKEN_NEQ, pos));
                advance();
                break;
            }

            default:
            tokens.clear();
            std::string error_msg = "Illegal char \'";
            error_msg += current_char;
            error_msg += "\'. At " + pos.get_pos() + "\n";
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

std::shared_ptr<Token> Lexer::make_identifier() {
    std::string id_str;
    while(current_char != NONE && (std::isalnum(current_char) || current_char == '_')) {
        id_str += current_char;
        advance();
    }
    std::string type = KEYWORDS.count(id_str) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
    return std::make_shared<TypedToken<std::string>>(type, pos, id_str);
}

/// --------------------
/// SymbolTable
/// --------------------

std::shared_ptr<Number> SymbolTable::get(std::string name) {
    if(symbols.count(name) == 0){
        if(parent != nullptr) {
            return parent->get(name);
        } else {
            return std::make_shared<ErrorNumber>(NUMBER_ERROR, "Identifier: \""+ name +"\" has not defined\n");
        }
    }
    return symbols[name];
}

void SymbolTable::set(std::string name, std::shared_ptr<Number> value) {
    symbols[name] = value;
}

void SymbolTable::erase(std::string name) {
    symbols.erase(name);
}

/// --------------------
/// Interpreter
/// --------------------

std::shared_ptr<Number> Interpreter::visit(std::shared_ptr<Node> node) {
    if(node->get_type() == NODE_NUMBER) {
        return visit_number(node);
    }
    if(node->get_type() == NODE_VARACCESS) {
        return visit_var_access(node);
    }
    if(node->get_type() == NODE_VARASSIGN) {
        return visit_var_assign(node);
    }
    if(node->get_type() == NODE_BINOP) {
        return visit_bin_op(node);
    }
    if(node->get_type() == NODE_UNARYOP) {
        return visit_unary_op(node);
    }
    if(node->get_type() == NODE_IF) {
        return visit_if(node);
    }
    return std::make_shared<ErrorNumber>(NUMBER_ERROR, "Fail to get result\n");
}

std::shared_ptr<Number> Interpreter::visit_number(std::shared_ptr<Node> node) {
    if(node->get_tok()->get_type() == TOKEN_INT) 
        return std::make_shared<TypedNumber<int64_t>>(NUMBER_INT, std::stoll(node->get_tok()->get_value()));
    else if(node->get_tok()->get_type() == TOKEN_FLOAT)
        return std::make_shared<TypedNumber<double>>(NUMBER_FLOAT, std::stod(node->get_tok()->get_value()));
    else
        return std::make_shared<TypedNumber<int64_t>>(NUMBER_INT, 0);
}

std::shared_ptr<Number> Interpreter::visit_var_access(std::shared_ptr<Node> node) {
    std::string var_name = node->get_name();
    return symbol_table.get(var_name);
}

std::shared_ptr<Number> Interpreter::visit_var_assign(std::shared_ptr<Node> node) {
    NodeList child = node->get_child();
    std::string var_name = node->get_name();
    std::shared_ptr<Number> value = visit(child[0]);
    if(value->get_type() == NUMBER_ERROR)
        return value;
    symbol_table.set(var_name, value);
    return symbol_table.get(var_name);
}

std::shared_ptr<Number> Interpreter::visit_bin_op(std::shared_ptr<Node> node) {
    NodeList child = node->get_child();
    std::shared_ptr<Number> a, b;
    a = visit(child[0]);
    b = visit(child[1]);
    if(a->get_type() == NUMBER_ERROR || b->get_type() == NUMBER_ERROR)
        return a->get_type() == NUMBER_ERROR ? a : b;
    return bin_op(a, b, node->get_tok());
}

std::shared_ptr<Number> Interpreter::visit_unary_op(std::shared_ptr<Node> node) {
    NodeList child = node->get_child();
    std::shared_ptr<Number> a = visit(child[0]);
    if(a->get_type() == NUMBER_ERROR)
        return a;
    return unary_op(a, node->get_tok());
}

std::shared_ptr<Number> Interpreter::visit_if(std::shared_ptr<Node> node) {
    NodeList child = node->get_child();
    std::shared_ptr<Number> cond = visit(child[0]);
    if(cond->get_type() == NUMBER_ERROR)
        return cond;
    if(std::stoll(cond->get_num()) == 1) {
        return visit(child[1]);
    } else if(child[2] != nullptr) {
        return visit(child[2]);
    }
    return std::make_shared<TypedNumber<int64_t>>(NUMBER_INT, 0);
}

std::shared_ptr<Number> Interpreter::bin_op(
    std::shared_ptr<Number> a, std::shared_ptr<Number> b, std::shared_ptr<Token> op
) {
    if(op->get_type() == TOKEN_ADD)
        return a + b;
    else if(op->get_type() == TOKEN_SUB)
        return a - b;
    else if(op->get_type() == TOKEN_MUL)
        return a * b;
    else if(op->get_type() == TOKEN_DIV)
        return a / b;
    else if(op->get_type() == TOKEN_MOD)
        return a % b;
    else if(op->get_type() == TOKEN_POW)
        return pow(a, b);
    else if(op->get_type() == TOKEN_EQUAL)
        return a == b;
    else if(op->get_type() == TOKEN_NEQ)
        return a != b;
    else if(op->get_type() == TOKEN_LESS)
        return a < b;
    else if(op->get_type() == TOKEN_GREATER)
        return a > b;
    else if(op->get_type() == TOKEN_LEQ)
        return a <= b;
    else if(op->get_type() == TOKEN_GEQ)
        return a >= b;
    else if(op->get_type() == TOKEN_KEYWORD && op->get_value() == "and")
        return a && b;
    else if(op->get_type() == TOKEN_KEYWORD && op->get_value() == "or")
        return a || b;
    
    return std::make_shared<ErrorNumber>(NUMBER_ERROR, "Not a binary op\n");
}

std::shared_ptr<Number> Interpreter::unary_op(std::shared_ptr<Number> a, std::shared_ptr<Token> op) {
    if(op->get_type() == TOKEN_ADD)
        return a;
    else if(op->get_type() == TOKEN_SUB)
        return -a;
    else if(op->get_type() == TOKEN_KEYWORD && op->get_value() == "not")
        return !a;
    return std::make_shared<ErrorNumber>(NUMBER_ERROR, "Not an unary op\n");
}

/// --------------------
/// Run
/// --------------------

std::string Run(std::string file_name, std::string text, SymbolTable &global_symbol_table) {
    Lexer lexer(file_name, text);
    TokenList tokens = lexer.make_tokens();
    if(tokens.empty()) return "";
    std::cout << "Tokens: " << tokens << "\n";

    Parser parser(tokens);
    std::shared_ptr<Node> ast = parser.parse();
    std::cout << "Nodes: " << ast->get_node() << "\n";
    if(ast->get_type() == NODE_ERROR) return "STOP";

    Interpreter interpreter(global_symbol_table);
    std::shared_ptr<Number> ret = interpreter.visit(ast);
    std::cout << "Result: " << ret->get_num() << "\n";
    if(ret->get_type() == NUMBER_ERROR) return "STOP";
    return "";
}