//
// Created by hlx on 2023/9/7.
//

#ifndef CPPLOX_PARSER_H
#define CPPLOX_PARSER_H

#include <vector>
#include "stmt.h"
#include "expr.h"

namespace cpplox {

    class ParseError : public std::exception {
    public:
        ParseError() = default;

        ~ParseError() override = default;
    };

    class Parser {
    private:
        std::vector<Token> tokens;
        int current = 0;
    public:
        explicit Parser(std::vector<Token> tokens);

        std::vector<Stmt*> parse();

        ~Parser();

    private:
        Stmt* declaration();

        bool isAtEnd();

        Token& peek();

        Stmt* statement();

        void synchronize();

        Token& advance();

        Token& previous();

        bool match(const std::vector<TokenType>& types);

        bool check(TokenType type);

        Stmt* classDeclaration();

        Token& consume(TokenType type, const std::string& message);

        ParseError error(Token& token, const std::string& message);

        Function* function(const std::string& kind);

        std::vector<Stmt*> block();

        Stmt* varDeclaration();

        Expr* expression();

        Expr* assignment();

        Expr* _or();

        Expr* _and();

        Expr* equality();

        Expr* comparison();

        Expr*  term();

        Expr* factor();

        Expr* unary();

        Expr* call();

        Expr* finishCall(Expr *callee);

        Expr* primary();

        Stmt* forStatement();

        Stmt* expressionStatement();

        Stmt* ifStatement();

        Stmt* printStatement();

        Stmt* returnStatement();

        Stmt* whileStatement();
    };

}

#endif //CPPLOX_PARSER_H
