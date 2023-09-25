//
// Created by hlx on 2023/9/6.
//

#ifndef CPPLOX_SCANNER_H
#define CPPLOX_SCANNER_H

#include <string>
#include <utility>
#include <vector>

#include "token.h"

namespace cpplox {

    class Scanner {
    private:
        std::string source;
        std::vector<Token> tokens;
        int start;
        int current;
        int line;
    public:
        explicit Scanner(std::string source);

        ~Scanner();

        std::vector<Token> scanTokens();

        void scanToken();

        char advance();

        bool isAtEnd();

        void addToken(TokenType type);

        void addToken(TokenType type, Object *literal);

        bool match(char expected);

        char peek();

        void string();

        void number();

        char peekNext();

        void identifier();
    };
}

#endif //CPPLOX_SCANNER_H
