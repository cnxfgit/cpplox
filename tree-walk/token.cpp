//
// Created by hlx on 2023/9/7.
//

#include <utility>
#include "token.h"

namespace cpplox {

    Token::Token(TokenType type, std::string lexeme, Object *literal, int line) {
        this->type = type;
        this->lexeme = std::move(lexeme);
        this->literal = literal;
        this->line = line;
    }

    Token::~Token() {
        delete this->literal;
    }

    Token::Token(Token &&other) noexcept {
        this->literal = other.literal;
        this->lexeme = std::move(other.lexeme);
        this->type = other.type;
        this->line = other.line;

        other.literal = nullptr;
    }

    Token &Token::operator=(Token &&other) noexcept {
        if (this != &other) {
            delete literal;

            this->literal = other.literal;
            this->lexeme = std::move(other.lexeme);
            this->line = other.line;
            this->type = other.type;

            other.literal = nullptr;
        }
        return *this;
    }

    Token::Token() {
        this->literal = nullptr;
        this->lexeme = std::string();
        this->type = TokenType::NIL;
        this->line = 0;
    }

}