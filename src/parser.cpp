/// --------------------
/// Parser
/// --------------------

#include "parser.h"
#include "color.h"
#include "lexer.h"
#include <string>
#include <algorithm>

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
    } else if(tok->get_type() == TOKEN_BUILTIN_CONST) {
        advance();
        std::shared_ptr<Token> ret{std::make_shared<TypedToken<int64_t>>(TOKEN_INT, tok->get_pos(), BUILTIN_CONST.at(tok->get_value()))};
        return std::make_shared<ValueNode>(ret);
    }  else if(tok->get_type() == TOKEN_BUILTIN_ALGO) {
        advance();
        return std::make_shared<VarAccessNode>(tok);
    } else if(tok->get_type() == TOKEN_LEFT_PAREN) {
        advance();
        std::shared_ptr<Node> e{expr()};
        if(current_tok->get_type() == TOKEN_RIGHT_PAREN) {
            advance();
            return e;
        }
        error_msg += "Expected \')\'" RESET "\n";
        std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
        return std::make_shared<ErrorNode>(error_token);
    } else if(tok->get_type() == TOKEN_IDENTIFIER) {
        advance();
        if(current_tok->get_type() == TOKEN_ASSIGN) {
            advance();
            std::shared_ptr<Node> ret = expr();
            if(ret->get_type() == NODE_ERROR) return ret;
            return std::make_shared<VarAssignNode>(tok->get_value(), ret);
        }
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
    } else if(tok->get_type() == TOKEN_LEFT_BRACE) {
        advance();
        return array_expr();
    }

    error_msg += "\"" RESET "\n" ;
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

std::shared_ptr<Node> Parser::array_expr() {
    NodeList ret;
    if(current_tok->get_type() == TOKEN_RIGHT_BRACE) {
        advance();
        return std::make_shared<ArrayNode>(ret);
    }
    ret.push_back(expr());
    if(ret.back()->get_type() == NODE_ERROR) return ret.back();
    while(current_tok->get_type() == TOKEN_COMMA) {
        advance();
        ret.push_back(expr());
        if(ret.back()->get_type() == NODE_ERROR) return ret.back();
    }
    if(current_tok->get_type() != TOKEN_RIGHT_BRACE) {
        std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected a \"}\"\n" RESET;
        std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
        return std::make_shared<ErrorNode>(error_token);
    }
    advance();
    return std::make_shared<ArrayNode>(ret);
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
    std::shared_ptr<Node> at{atom()};
    if(at->get_type() == NODE_ERROR) return at;
    if(current_tok->get_type() != TOKEN_LEFT_PAREN && current_tok->get_type() != TOKEN_LEFT_SQUARE) {
        return at;
    }
    std::shared_ptr<Node> ret;
    while(current_tok->get_type() == TOKEN_LEFT_PAREN || current_tok->get_type() == TOKEN_LEFT_SQUARE) {
        while(current_tok->get_type() == TOKEN_LEFT_PAREN) {
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
            ret = std::make_shared<AlgorithmCallNode>(at, args);
            at = ret;
        }
        while(current_tok->get_type() == TOKEN_LEFT_SQUARE) {
            advance();
            std::shared_ptr<Node> index{expr()};
            if(index->get_type() == NODE_ERROR) return index;
            if(current_tok->get_type() != TOKEN_RIGHT_SQUARE) {
                std::string error_msg = Color(0xFF, 0x39, 0x6E).get() + "Expected a \"]\"" RESET "\n";
                std::shared_ptr<Token> error_token = std::make_shared<ErrorToken>(TOKEN_ERROR, current_tok->get_pos(), error_msg);
                return std::make_shared<ErrorNode>(error_token);
            }
            advance();
            ret = std::make_shared<ArrayAccessNode>(at, index);
            at = ret;
        }
    }
    if(current_tok->get_type() == TOKEN_ASSIGN) {
        advance();
        std::shared_ptr<Node> val = expr();
        if(val->get_type() == NODE_ERROR) return val;
        return std::make_shared<ArrayAssignNode>(ret, val);
    }
    return ret;
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