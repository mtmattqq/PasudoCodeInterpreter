#include "pseudo.h"
#include "color.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <algorithm>

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
    ss << type << ": " << value << RESET;
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
/// Value
/// --------------------

std::ostream& operator<<(std::ostream &out, Value &number) {
    out << number.get_num();
    return out;
}

template<typename T>
std::string TypedValue<T>::get_num() {
    std::stringstream ss;
    ss << value;
    std::string ret;
    std::getline(ss, ret);
    return ret;
}

std::shared_ptr<Value> AlgoValue::execute(NodeList args, SymbolTable *parent) {
    SymbolTable sym(parent);
    Interpreter interpreter(sym);
    if(args.size() < value->get_toks().size()) {
        return std::make_shared<ErrorValue>(VALUE_ERROR, Color(0xFF, 0x39, 0x6E).get() + "Too few arguments" RESET);
    } else if(args.size() > value->get_toks().size()) {
        return std::make_shared<ErrorValue>(VALUE_ERROR, Color(0xFF, 0x39, 0x6E).get() + "Too many arguments" RESET);
    }

    TokenList args_name = value->get_toks();
    for(int i = 0; i < args.size(); ++i) {
        std::shared_ptr<Value> v = interpreter.visit(args[i]);
        sym.set(args_name[i]->get_value(), v);
    }

    NodeList algo_body = value->get_child();
    std::shared_ptr<Value> ret;
    for(int i = 0; i < algo_body.size(); ++i) {
        ret = interpreter.visit(algo_body[i]);
    }
    return ret;
}


std::shared_ptr<Value> operator+(std::shared_ptr<Value> a, std::shared_ptr<Value> b) {
    if(a->get_type() == VALUE_FLOAT || b->get_type() == VALUE_FLOAT)
        return std::make_shared<TypedValue<double>>(
            VALUE_FLOAT, std::stod(a->get_num()) + std::stod(b->get_num()));
    else if(a->get_type() == VALUE_INT && b->get_type() == VALUE_INT)
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, std::stoll(a->get_num()) + std::stoll(b->get_num()));
    else if(a->get_type() == VALUE_STRING && b->get_type() == VALUE_STRING)
        return std::make_shared<TypedValue<std::string>>(
            VALUE_STRING, a->get_num() + b->get_num());
    else
        return std::make_shared<ErrorValue>(
            VALUE_ERROR, Color(0xFF, 0x39, 0x6E).get() + "Runtime ERROR: ADD operation can only apply on number or two string\n" RESET);
}

std::shared_ptr<Value> operator-(std::shared_ptr<Value> a, std::shared_ptr<Value> b) {
    if(a->get_type() == VALUE_FLOAT || b->get_type() == VALUE_FLOAT)
        return std::make_shared<TypedValue<double>>(
            VALUE_FLOAT, std::stod(a->get_num()) - std::stod(b->get_num()));
    else if(a->get_type() == VALUE_INT && b->get_type() == VALUE_INT)
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, std::stoll(a->get_num()) - std::stoll(b->get_num()));
    else
        return std::make_shared<ErrorValue>(
            VALUE_ERROR, Color(0xFF, 0x39, 0x6E).get() + "Runtime ERROR: SUB operation can only apply on number\n" RESET);
}

std::shared_ptr<Value> operator*(std::shared_ptr<Value> a, std::shared_ptr<Value> b) {
    if(a->get_type() == VALUE_FLOAT || b->get_type() == VALUE_FLOAT)
        return std::make_shared<TypedValue<double>>(
            VALUE_FLOAT, std::stod(a->get_num()) * std::stod(b->get_num()));
    else if(a->get_type() == VALUE_INT && b->get_type() == VALUE_INT)
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, std::stoll(a->get_num()) * std::stoll(b->get_num()));
    else if(a->get_type() == VALUE_STRING && b->get_type() == VALUE_INT) {
        std::string ret, str_a{a->get_num()};
        int64_t times{stoll(b->get_num())};
        for(int i{0}; i < times; ++i)
            ret += str_a;
        return std::make_shared<TypedValue<std::string>>(
            VALUE_STRING, ret);
    } else
        return std::make_shared<ErrorValue>(
            VALUE_ERROR, Color(0xFF, 0x39, 0x6E).get() + "Runtime ERROR: MUL operation can only apply on number or string and int\n" RESET);
}

std::shared_ptr<Value> operator/(std::shared_ptr<Value> a, std::shared_ptr<Value> b) {
    if(std::stod(b->get_num()) == 0.0) 
        return std::make_shared<ErrorValue>(VALUE_ERROR, Color(0xFF, 0x39, 0x6E).get() + "Runtime ERROR: DIV by 0\n" RESET);
    if(a->get_type() == VALUE_FLOAT || b->get_type() == VALUE_FLOAT)
        return std::make_shared<TypedValue<double>>(
            VALUE_FLOAT, std::stod(a->get_num()) / std::stod(b->get_num()));
    else if(a->get_type() == VALUE_INT && b->get_type() == VALUE_INT)
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, std::stoll(a->get_num()) / std::stoll(b->get_num()));
    else
        return std::make_shared<ErrorValue>(
            VALUE_ERROR, Color(0xFF, 0x39, 0x6E).get() + "Runtime ERROR: DIV operation can only apply on number\n" RESET);
}

std::shared_ptr<Value> operator%(std::shared_ptr<Value> a, std::shared_ptr<Value> b) {
    if(a->get_type() != VALUE_INT || b->get_type() != VALUE_INT) 
        return std::make_shared<ErrorValue>(VALUE_ERROR, Color(0xFF, 0x39, 0x6E).get() + "Float doesn't have \"%\" operation\n" RESET);
    return std::make_shared<TypedValue<int64_t>>(
        VALUE_INT, std::stoll(a->get_num()) % std::stoll(b->get_num()));
}

std::shared_ptr<Value> operator==(std::shared_ptr<Value> a, std::shared_ptr<Value> b) {
    if(a->get_type() == VALUE_FLOAT || b->get_type() == VALUE_FLOAT)
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, std::stod(a->get_num()) == std::stod(b->get_num()));
    else
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, a->get_num() == b->get_num());
}

std::shared_ptr<Value> operator!=(std::shared_ptr<Value> a, std::shared_ptr<Value> b) {
    if(a->get_type() == VALUE_FLOAT || b->get_type() == VALUE_FLOAT)
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, std::stod(a->get_num()) != std::stod(b->get_num()));
    else
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, a->get_num() != b->get_num());
}

std::shared_ptr<Value> operator<(std::shared_ptr<Value> a, std::shared_ptr<Value> b) {
    if(a->get_type() == VALUE_FLOAT || b->get_type() == VALUE_FLOAT)
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, std::stod(a->get_num()) < std::stod(b->get_num()));
    else
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, a->get_num() < b->get_num());
}

std::shared_ptr<Value> operator>(std::shared_ptr<Value> a, std::shared_ptr<Value> b) {
    if(a->get_type() == VALUE_FLOAT || b->get_type() == VALUE_FLOAT)
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, std::stod(a->get_num()) > std::stod(b->get_num()));
    else
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, a->get_num() > b->get_num());
}

std::shared_ptr<Value> operator<=(std::shared_ptr<Value> a, std::shared_ptr<Value> b) {
    if(a->get_type() == VALUE_FLOAT || b->get_type() == VALUE_FLOAT)
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, std::stod(a->get_num()) <= std::stod(b->get_num()));
    else
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, a->get_num() <= b->get_num());
}

std::shared_ptr<Value> operator>=(std::shared_ptr<Value> a, std::shared_ptr<Value> b) {
    if(a->get_type() == VALUE_FLOAT || b->get_type() == VALUE_FLOAT)
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, std::stod(a->get_num()) >= std::stod(b->get_num()));
    else
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, a->get_num() >= b->get_num());
}

std::shared_ptr<Value> operator&&(std::shared_ptr<Value> a, std::shared_ptr<Value> b) {
    if(a->get_type() == VALUE_FLOAT || b->get_type() == VALUE_FLOAT)
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, std::stod(a->get_num()) != 0 && std::stod(b->get_num()) != 0);
    else
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, std::stoll(a->get_num()) && std::stoll(b->get_num()));
}

std::shared_ptr<Value> operator||(std::shared_ptr<Value> a, std::shared_ptr<Value> b) {
    if(a->get_type() == VALUE_FLOAT || b->get_type() == VALUE_FLOAT)
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, std::stod(a->get_num()) != 0 || std::stod(b->get_num()) != 0);
    else
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, std::stoll(a->get_num()) || std::stoll(b->get_num()));
}

std::shared_ptr<Value> operator-(std::shared_ptr<Value> a) {
    if(a->get_type() == VALUE_FLOAT)
        return std::make_shared<TypedValue<double>>(VALUE_FLOAT, 0 - stod(a->get_num()));
    else
        return std::make_shared<TypedValue<int64_t>>(VALUE_INT, 0 - stoll(a->get_num()));
}

std::shared_ptr<Value> operator!(std::shared_ptr<Value> a) {
    if(a->get_type() == VALUE_FLOAT)
        return std::make_shared<TypedValue<double>>(VALUE_FLOAT, stod(a->get_num()) == 0);
    else
        return std::make_shared<TypedValue<int64_t>>(VALUE_INT, stoll(a->get_num()) == 0);
}

std::shared_ptr<Value> pow(std::shared_ptr<Value> a, std::shared_ptr<Value> b) {
    if(std::stod(a->get_num()) == 0.0 && std::stod(b->get_num()) == 0.0) 
        return std::make_shared<ErrorValue>(VALUE_ERROR, Color(0xFF, 0x39, 0x6E).get() + "Runtime ERROR: 0 to the 0\n" RESET);
    if(a->get_type() == VALUE_FLOAT || b->get_type() == VALUE_FLOAT)
        return std::make_shared<TypedValue<double>>(
            VALUE_FLOAT, std::pow(std::stod(a->get_num()), std::stod(b->get_num())));
    else
        return std::make_shared<TypedValue<int64_t>>(
            VALUE_INT, std::pow(std::stoll(a->get_num()), std::stoll(b->get_num())));
}

/// --------------------
/// Node
/// --------------------

std::string ValueNode::get_node() {
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

std::string ForNode::get_node() {
    std::stringstream ss;
    ss << "(FOR " << var_assign->get_node() << " TO " << end_value->get_node();
    if(step_value != nullptr)
        ss << " STEP " << step_value->get_node();
    ss << " DO " << body_node->get_node();
    ss << ")";
    std::string ret;
    std::getline(ss, ret);
    return ret;
}

std::string WhileNode::get_node() {
    std::stringstream ss;
    ss << "(WHILE " << condition->get_node() << " DO " << body_node->get_node() << ")";
    std::string ret;
    std::getline(ss, ret);
    return ret;
}

std::string RepeatNode::get_node() {
    std::stringstream ss;
    ss << "(REPEAT " << body_node->get_node() << " UNTIL " << condition->get_node() << ")";
    std::string ret;
    std::getline(ss, ret);
    return ret;
}

std::string AlgorithmDefNode::get_node() {
    std::stringstream ss;
    ss << "ALGORITHM " << algo_name->get_tok() << "(";
    if(!args_name.empty()) {
        ss << args_name[0]->get_tok();
    }
    for(int i{1}; i < args_name.size(); ++i) {
        ss << ", " << args_name[i]->get_tok();
    }
    ss << "):\n";
    for(auto exp : body_node) {
        ss << TAB << exp->get_node() << "\n";
    }
    std::string ret, line;
    while(std::getline(ss, line)) {
        ret += line;
        ret += "\n";
    }
    return ret;
}

std::string AlgorithmCallNode::get_node() {
    std::stringstream ss;
    ss << "(CALL ALGORITHM " << call_node->get_name() << "(";
    if(!args.empty()) {
        ss << args[0]->get_node();
    }
    for(int i{1}; i < args.size(); ++i) {
        ss << ", " << args[i]->get_node();
    }
    ss << "))";
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
        return std::make_shared<ValueNode>(tok);
    } else if(tok->get_type() == TOKEN_STRING) {
        advance();
        return std::make_shared<ValueNode>(tok);
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
    } else if(tok->get_type() == TOKEN_KEYWORD && tok->get_value() == "for") {
        advance();
        return for_expr();
    } else if(tok->get_type() == TOKEN_KEYWORD && tok->get_value() == "while") {
        advance();
        return while_expr();
    } else if(tok->get_type() == TOKEN_KEYWORD && tok->get_value() == "repeat") {
        advance();
        return repeat_expr();
    } else if(tok->get_type() == TOKEN_KEYWORD && tok->get_value() == "Algorithm") {
        advance();
        return algo_def();
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
    return bin_op(std::bind(&Parser::factor, this), {TOKEN_MUL, TOKEN_DIV, TOKEN_MOD}, std::bind(&Parser::factor, this));
}

std::shared_ptr<Node> Parser::expr() {
    if(current_tok->get_type() == TOKEN_KEYWORD && current_tok->get_value() == "var") {
        advance();
        if(current_tok->get_type() != TOKEN_IDENTIFIER) {
            std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected Identifier\n" RESET;
            std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
            return std::make_shared<ErrorNode>(error_token);
        }
        std::string var_name = current_tok->get_value();
        advance();
        if(current_tok->get_type() != TOKEN_ASSIGN) {
            std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected \"<-\"\n" RESET;
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
    std::shared_ptr<Node> condition = expr();
    if(condition->get_type() == NODE_ERROR) return condition;
    if(!(current_tok->get_type() == TOKEN_KEYWORD && current_tok->get_value() == "then")) {
        std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected \"then\"\n" RESET;
        std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
        return std::make_shared<ErrorNode>(error_token);
    }
    advance();
    std::shared_ptr<Node> exp = expr();
    if(exp->get_type() == NODE_ERROR) return exp;
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
    return std::make_shared<IfNode>(condition, exp, els);
}

std::shared_ptr<Node> Parser::for_expr() {
    std::shared_ptr<Token> var_name = current_tok;
    if(current_tok->get_type() != TOKEN_IDENTIFIER) {
        std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected \"an identifier\"\n" RESET;
        std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
        return std::make_shared<ErrorNode>(error_token);
    }
    advance();
    if(current_tok->get_type() != TOKEN_ASSIGN) {
        std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected \"<-\"\n" RESET;
        std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
        return std::make_shared<ErrorNode>(error_token);
    }
    advance();
    std::shared_ptr<Node> start_value = expr();
    if(start_value->get_type() == NODE_ERROR) return start_value;
    std::shared_ptr<Node> var_assign = std::make_shared<VarAssignNode>(var_name->get_value(), start_value);
    
    if(!(current_tok->get_type() == TOKEN_KEYWORD && current_tok->get_value() == "to")) {
        std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected \"to\"\n" RESET;
        std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
        return std::make_shared<ErrorNode>(error_token);
    }
    advance();
    std::shared_ptr<Node> end_value = expr();
    if(end_value->get_type() == NODE_ERROR) return end_value;

    std::shared_ptr<Node> step_value = nullptr;
    if(current_tok->get_type() == TOKEN_KEYWORD && current_tok->get_value() == "step") {
        advance();
        step_value = expr();
        if(step_value->get_type() == NODE_ERROR) return step_value;
    }
    if(!(current_tok->get_type() == TOKEN_KEYWORD && current_tok->get_value() == "do")) {
        std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected \"do\"\n" RESET;
        std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
        return std::make_shared<ErrorNode>(error_token);
    }
    advance();
    std::shared_ptr<Node> body_node = expr();
    if(body_node->get_type() == NODE_ERROR) return body_node;
    return std::make_shared<ForNode>(var_assign, end_value, step_value, body_node);
}

std::shared_ptr<Node> Parser::while_expr() {
    std::shared_ptr<Node> condition = expr();
    if(condition->get_type() == NODE_ERROR) return condition;
    if(!(current_tok->get_type() == TOKEN_KEYWORD && current_tok->get_value() == "do")) {
        std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected \"do\"\n" RESET;
        std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
        return std::make_shared<ErrorNode>(error_token);
    }
    advance();
    std::shared_ptr<Node> body_node = expr();
    if(body_node->get_type() == NODE_ERROR) return body_node;
    return std::make_shared<WhileNode>(condition, body_node);
}

std::shared_ptr<Node> Parser::repeat_expr() {
    std::shared_ptr<Node> body_node = expr();
    if(body_node->get_type() == NODE_ERROR) return body_node;
    if(!(current_tok->get_type() == TOKEN_KEYWORD && current_tok->get_value() == "until")) {
        std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected \"until\"\n" RESET;
        std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
        return std::make_shared<ErrorNode>(error_token);
    }
    advance();
    std::shared_ptr<Node> condition = expr();
    if(condition->get_type() == NODE_ERROR) return condition;
    return std::make_shared<RepeatNode>(body_node, condition);
}

std::shared_ptr<Node> Parser::algo_def() {
    std::shared_ptr<Token> algo_name = current_tok;
    if(current_tok->get_type() == TOKEN_IDENTIFIER) {
        advance();
        if(current_tok->get_type() != TOKEN_LEFT_PAREN) {
            std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected a \"(\"\n" RESET;
            std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
            return std::make_shared<ErrorNode>(error_token);
        }
    } else if(current_tok->get_type() != TOKEN_LEFT_PAREN) {
        algo_name = std::make_shared<TypedToken<std::string>>(TOKEN_IDENTIFIER, current_tok->get_pos(), "Anonymous");
    } else {
        std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected an \"identifier\" or \"(\"\n" RESET;
        std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
        return std::make_shared<ErrorNode>(error_token);
    }
    advance();
    TokenList args_name;
    if(current_tok->get_type() == TOKEN_IDENTIFIER) {
        args_name.push_back(current_tok);
        advance();
        while(current_tok->get_type() == TOKEN_COMMA) {
            advance();
            if(current_tok->get_type() != TOKEN_IDENTIFIER) {
                std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected an \"identifier\" or a \"(\"\n" RESET;
                std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
                return std::make_shared<ErrorNode>(error_token);
            }
            args_name.push_back(current_tok);
            advance();
        }
    }

    if(current_tok->get_type() != TOKEN_RIGHT_PAREN) {
        std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected a \")\"\n" RESET;
        std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
        return std::make_shared<ErrorNode>(error_token);
    }
    advance();
    if(current_tok->get_type() != TOKEN_COLON) {
        std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected a \":\"\n" RESET;
        std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
        return std::make_shared<ErrorNode>(error_token);
    }
    advance();
    NodeList body_node{expr()};
    if(body_node[0]->get_type() == NODE_ERROR) return body_node[0];
    return std::make_shared<AlgorithmDefNode>(algo_name, args_name, body_node);
}

std::shared_ptr<Node> Parser::pow() {
    return bin_op(std::bind(&Parser::call, this), {TOKEN_POW}, std::bind(&Parser::factor, this));
}

std::shared_ptr<Node> Parser::call() {
    std::shared_ptr<Node> at = atom();
    if(at->get_type() == NODE_ERROR) return at;
    if(current_tok->get_type() != TOKEN_LEFT_PAREN) {
        return at;
    }
    advance();
    NodeList args;
    if(current_tok->get_type() == TOKEN_RIGHT_PAREN) {
        advance();
    } else {
        args.push_back(expr());
        if(args.back()->get_type() == NODE_ERROR) return args.back();
        while(current_tok->get_type() == TOKEN_COMMA) {
            advance();
            args.push_back(expr());
            if(args.back()->get_type() == NODE_ERROR) return args.back();
        }
        if(current_tok->get_type() != TOKEN_RIGHT_PAREN) {
            std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected a \")\"\n" RESET;
            std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
            return std::make_shared<ErrorNode>(error_token);
        }
    }
    advance();
    return std::make_shared<AlgorithmCallNode>(at, args);
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
        if(current_char == '\"') {
            std::shared_ptr<Token> new_token = make_string();
            if(new_token->get_type() == TOKEN_ERROR) {
                tokens.clear();
                tokens.push_back(new_token);
                return tokens;
            }
            tokens.push_back(new_token);
            continue;
        }

        switch(current_char) {
            case ' ': case '\t': case '\n':
            advance(); break;
            
            case '+': case '-': case '*': case '/': case '%': case '^': case '(': case ')':
            case '=': case ',': case ':':
            tokens.push_back(std::make_shared<Token>(TO_TOKEN_TYPE.at(current_char), pos));
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

std::shared_ptr<Token> Lexer::make_string() {
    advance();
    std::string ret;
    while(current_char != NONE && current_char != '\"') {
        if(current_char == '\\') {
            advance();
            if(ESCAPE_CHAR.count(current_char))
                ret += ESCAPE_CHAR.at(current_char);
            else
                return std::make_shared<ErrorToken>(
                    TOKEN_ERROR, pos, Color(0xFF, 0x39, 0x6E).get() + "Unknown char after \'\\\'" RESET);
            advance();
            continue;
        }
        ret += current_char;
        advance();
    }
    if(current_char != '\"')
        return std::make_shared<ErrorToken>(
            TOKEN_ERROR, pos, Color(0xFF, 0x39, 0x6E).get() + "Expected \'\"\'" RESET);
    advance();
    return std::make_shared<TypedToken<std::string>>(TOKEN_STRING, pos, ret);
}

/// --------------------
/// SymbolTable
/// --------------------

std::shared_ptr<Value> SymbolTable::get(std::string name) {
    if(symbols.count(name) == 0){
        if(parent != nullptr) {
            return parent->get(name);
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

/// --------------------
/// Interpreter
/// --------------------

std::shared_ptr<Value> Interpreter::visit(std::shared_ptr<Node> node) {
    if(node->get_type() == NODE_VALUE) {
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
    if(node->get_type() == NODE_FOR) {
        return visit_for(node);
    }
    if(node->get_type() == NODE_WHILE) {
        return visit_while(node);
    }
    if(node->get_type() == NODE_REPEAT) {
        return visit_repeat(node);
    }
    if(node->get_type() == NODE_ALGODEF) {
        return visit_algo_def(node);
    }
    if(node->get_type() == NODE_ALGOCALL) {
        return visit_algo_call(node);
    }
    return std::make_shared<ErrorValue>(VALUE_ERROR, "Fail to get result\n");
}

std::shared_ptr<Value> Interpreter::visit_number(std::shared_ptr<Node> node) {
    if(node->get_tok()->get_type() == TOKEN_INT) 
        return std::make_shared<TypedValue<int64_t>>(VALUE_INT, std::stoll(node->get_tok()->get_value()));
    else if(node->get_tok()->get_type() == TOKEN_FLOAT)
        return std::make_shared<TypedValue<double>>(VALUE_FLOAT, std::stod(node->get_tok()->get_value()));
    else if(node->get_tok()->get_type() == TOKEN_STRING)
        return std::make_shared<TypedValue<std::string>>(VALUE_STRING, node->get_tok()->get_value());
    else
        return std::make_shared<ErrorValue>(VALUE_ERROR, "Not a value type\n");
}

std::shared_ptr<Value> Interpreter::visit_var_access(std::shared_ptr<Node> node) {
    std::string var_name = node->get_name();
    return symbol_table.get(var_name);
}

std::shared_ptr<Value> Interpreter::visit_var_assign(std::shared_ptr<Node> node) {
    NodeList child = node->get_child();
    std::string var_name = node->get_name();
    std::shared_ptr<Value> value = visit(child[0]);
    if(value->get_type() == VALUE_ERROR)
        return value;
    symbol_table.set(var_name, value);
    return symbol_table.get(var_name);
}

std::shared_ptr<Value> Interpreter::visit_bin_op(std::shared_ptr<Node> node) {
    NodeList child = node->get_child();
    std::shared_ptr<Value> a, b;
    a = visit(child[0]);
    b = visit(child[1]);
    if(a->get_type() == VALUE_ERROR || b->get_type() == VALUE_ERROR)
        return a->get_type() == VALUE_ERROR ? a : b;
    return bin_op(a, b, node->get_tok());
}

std::shared_ptr<Value> Interpreter::visit_unary_op(std::shared_ptr<Node> node) {
    NodeList child = node->get_child();
    std::shared_ptr<Value> a = visit(child[0]);
    if(a->get_type() == VALUE_ERROR)
        return a;
    return unary_op(a, node->get_tok());
}

std::shared_ptr<Value> Interpreter::visit_if(std::shared_ptr<Node> node) {
    NodeList child = node->get_child();
    std::shared_ptr<Value> cond = visit(child[0]);
    if(cond->get_type() == VALUE_ERROR)
        return cond;
    if(std::stoll(cond->get_num()) == 1) {
        return visit(child[1]);
    } else if(child[2] != nullptr) {
        return visit(child[2]);
    }
    return std::make_shared<TypedValue<int64_t>>(VALUE_INT, 0);
}

std::shared_ptr<Value> Interpreter::visit_for(std::shared_ptr<Node> node) {
    NodeList child = node->get_child();
    std::shared_ptr<Value> i = visit(child[0]);
    std::shared_ptr<Value> step;
    if(child[2] != nullptr) {
        step = visit(child[2]);
    } else {
        step = std::make_shared<TypedValue<int64_t>>(VALUE_INT, 1);
    }
    std::shared_ptr<Value> end_value = visit(child[1]);
    std::function<bool(std::shared_ptr<Value>, std::shared_ptr<Value>)> condition;
    if(stod(step->get_num()) > 0) {
        condition = [](std::shared_ptr<Value> i, std::shared_ptr<Value> end) -> bool {
            return std::stoll((i <= end)->get_num()) == 1;
        };
    } else if(std::stod(step->get_num()) < 0) {
        condition = [](std::shared_ptr<Value> i, std::shared_ptr<Value> end) -> bool {
            return std::stoll((i >= end)->get_num()) == 1;
        };
    } else {
        return std::make_shared<ErrorValue>(VALUE_ERROR, "Infinite for loop\n");
    }

    while(condition(i, end_value)) {
        std::shared_ptr<Value> ret = visit(child[3]);
        if(ret->get_type() == VALUE_ERROR) 
            return ret;
        symbol_table.set(child[0]->get_name(), i + step);
        i = symbol_table.get(child[0]->get_name());
    }
    return std::make_shared<TypedValue<int64_t>>(VALUE_INT, 0);
}

std::shared_ptr<Value> Interpreter::visit_while(std::shared_ptr<Node> node) {
    NodeList child = node->get_child();
    while(std::stoll(visit(child[0])->get_num()) == 1) {
        std::shared_ptr<Value> ret = visit(child[1]);
        if(ret->get_type() == VALUE_ERROR) 
            return ret;
    }
    return std::make_shared<TypedValue<int64_t>>(VALUE_INT, 0);
}

std::shared_ptr<Value> Interpreter::visit_repeat(std::shared_ptr<Node> node) {
    NodeList child = node->get_child();
    do {
        std::shared_ptr<Value> ret = visit(child[0]);
        if(ret->get_type() == VALUE_ERROR) 
            return ret;
    } while(std::stoll(visit(child[1])->get_num()) == 0);
    return std::make_shared<TypedValue<int64_t>>(VALUE_INT, 0);
}

std::shared_ptr<Value> Interpreter::visit_algo_def(std::shared_ptr<Node> node) {
    std::string algo_name = node->get_name();
    std::shared_ptr<Value> value = std::make_shared<AlgoValue>(algo_name, node);
    symbol_table.set(algo_name, value);
    return symbol_table.get(algo_name);
}

std::shared_ptr<Value> Interpreter::visit_algo_call(std::shared_ptr<Node> node) {
    NodeList child = node->get_child();
    std::string algo_name = node->get_name();
    std::shared_ptr<Value> algo = symbol_table.get(algo_name);
    return algo->execute(child, &symbol_table);
}

std::shared_ptr<Value> Interpreter::bin_op(
    std::shared_ptr<Value> a, std::shared_ptr<Value> b, std::shared_ptr<Token> op
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
    
    return std::make_shared<ErrorValue>(VALUE_ERROR, "Not a binary op\n");
}

std::shared_ptr<Value> Interpreter::unary_op(std::shared_ptr<Value> a, std::shared_ptr<Token> op) {
    if(op->get_type() == TOKEN_ADD)
        return a;
    else if(op->get_type() == TOKEN_SUB)
        return -a;
    else if(op->get_type() == TOKEN_KEYWORD && op->get_value() == "not")
        return !a;
    return std::make_shared<ErrorValue>(VALUE_ERROR, "Not an unary op\n");
}

/// --------------------
/// Run
/// --------------------

std::string Run(std::string file_name, std::string text, SymbolTable &global_symbol_table) {
    Lexer lexer(file_name, text);
    TokenList tokens = lexer.make_tokens();
    if(tokens.empty()) return "";
    if(tokens[0]->get_type() == TOKEN_ERROR)
        std::cout << "Tokens: " << tokens << "\n";

    Parser parser(tokens);
    std::shared_ptr<Node> ast = parser.parse();
    if(ast->get_type() == NODE_ERROR)
        std::cout << "Nodes: " << ast->get_node() << "\n";
    if(ast->get_type() == NODE_ERROR) return "ABORT";

    Interpreter interpreter(global_symbol_table);
    std::shared_ptr<Value> ret = interpreter.visit(ast);
    std::cout << ret->get_num() << "\n";
    if(ret->get_type() == VALUE_ERROR) return "ABORT";
    return "";
}
