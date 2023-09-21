//
// Created by hlx on 2023/9/7.
//

#ifndef CPPLOX_TOKEN_H
#define CPPLOX_TOKEN_H

#include <string>
#include "object.h"

namespace cpplox {

    enum class TokenType {
        // Single-character tokens.
        LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
        COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

        // One or two character tokens.
        BANG, BANG_EQUAL,
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL,

        // Literals.
        IDENTIFIER, STRING, NUMBER,

        // Keywords.
        AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
        PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

        END
    };

    class Token {
    public:
        TokenType type;
        std::string lexeme;
        Object *literal;
        int line;

        Token();

        Token(TokenType type, std::string lexeme, Object *literal, int line);

        Token(Token &&other) noexcept;

        Token& operator=(Token &&other) noexcept;

        ~Token();
    };
}

#endif //CPPLOX_TOKEN_H
