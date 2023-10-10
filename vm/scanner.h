//
// Created by hlx on 2023/10/4.
//

#ifndef CPPLOX_SCANNER_H
#define CPPLOX_SCANNER_H

namespace cpplox{
// 令牌类型枚举
     enum TokenType{
        // 单字符标记
        TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
        TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
        TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
        TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
        // 一个或者两个的字符标记
        TOKEN_BANG, TOKEN_BANG_EQUAL,
        TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
        TOKEN_GREATER, TOKEN_GREATER_EQUAL,
        TOKEN_LESS, TOKEN_LESS_EQUAL,
        // 字面量
        TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,
        // 关键字
        TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
        TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
        TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
        TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,
        // 错误令牌或者结束符
        TOKEN_ERROR, TOKEN_EOF
    } ;


// 词法令牌
     struct Token{
        TokenType type;     // 令牌类型
        const char* start;  // 起点指针
        int length;         // 长度
        int line;           // 行号
    } ;

    class Scanner {
    private:
        const char *start;      // 指向起点的指针
        const char *current;    // 指向当前的指针
        int line;               // 行号

    public:
        explicit Scanner(const char* source);

        bool isAtEnd();

        char advance();

        char peek();

        char peekNext();

        bool match(char expected);

        Token makeToken(TokenType type);

        Token errorToken(const char *message) const;

        void skipWhitespace();

        TokenType checkKeyword(int p_start, int length, const char *rest, TokenType type);

        TokenType identifierType();

        Token identifier();

        Token number();

        Token string();

        Token scanToken();

        ~Scanner() = default;
    };

    // 扫描令牌
    Token scanToken();
}

#endif //CPPLOX_SCANNER_H
