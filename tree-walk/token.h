//
// Created by hlx on 2023/9/7.
//

#ifndef CPPLOX_TOKEN_H
#define CPPLOX_TOKEN_H

#include <string>

namespace cpplox {

    class Literal {
    public:
        virtual std::string toString() = 0;

        virtual ~Literal() = default;
    };

    class Boolean : public Literal {
        bool value;
    public:
        explicit Boolean(bool value);

        std::string toString() override;

        ~Boolean() override;
    };

    class Number : public Literal {
        double value;
    public:
        explicit Number(double value);

        std::string toString() override;

        ~Number() override;
    };

    class String : public Literal {
        std::string value;
    public:
        explicit String(std::string value);

        std::string toString() override;

        ~String() override;
    };

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
        Literal *literal;
        int line;

        Token(TokenType type, std::string lexeme, Literal *literal, int line);

        Token(Token &&other) noexcept;

        ~Token();
    };
}

#endif //CPPLOX_TOKEN_H
