/// --------------------
/// Lexer
/// --------------------

#include "lexer.h"
#include "color.h"
#include <string>

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
            case '=': case ',': case ':': case '{': case '}': case '[': case ']':
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
    std::string type;
    if(KEYWORDS.count(id_str))
        type = TOKEN_KEYWORD;
    else if(BUILTIN_CONST.count(id_str))
        type = TOKEN_BUILTIN_CONST;
    else if(BUILTIN_ALGO.count(id_str))
        type = TOKEN_BUILTIN_ALGO;
    else 
        type = TOKEN_IDENTIFIER;
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