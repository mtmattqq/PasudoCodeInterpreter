/// --------------------
/// Parser
/// --------------------

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <memory>
#include <functional>
#include "node.h"
#include "token.h"

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

#endif