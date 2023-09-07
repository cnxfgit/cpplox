//
// Created by hlx on 2023/9/7.
//

#include <utility>
#include "token.h"

namespace cpplox {

    Token::Token(TokenType type, std::string lexeme, Literal *literal, int line) {
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

    Boolean::Boolean(bool value) {
        this->value = value;
    }

    std::string Boolean::toString() {
        return value ? "true" : "false";
    }

    Boolean::~Boolean() = default;

    Number::Number(double value) {
        this->value = value;
    }

    std::string Number::toString() {
        return std::to_string(value);
    }

    Number::~Number() = default;

    String::String(std::string value) {
        this->value = std::move(value);
    }

    std::string String::toString() {
        return value;
    }

    String::~String() = default;

}