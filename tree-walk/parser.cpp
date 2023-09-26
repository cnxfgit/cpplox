//
// Created by hlx on 2023/9/7.
//

#include "parser.h"

namespace cpplox {

    void error(Token &token, const std::string &message);

    Parser::Parser(std::vector<Token> tokens) {
        this->tokens = std::move(tokens);
    }

    std::vector<Stmt *> Parser::parse() {
        std::vector<Stmt *> statements;
        while (!isAtEnd()) {
            statements.push_back(declaration());
        }

        return std::move(statements);
    }

    Stmt *Parser::declaration() {
        try {
            if (match(std::vector<TokenType>{TokenType::CLASS}))
                return classDeclaration();
            if (match(std::vector<TokenType>{TokenType::FUN}))
                return function("function");
            if (match(std::vector<TokenType>{TokenType::VAR}))
                return varDeclaration();

            return statement();
        } catch (ParseError &e) {
            synchronize();
            return nullptr;
        }
    }

    bool Parser::isAtEnd() {
        return peek().type == TokenType::END;
    }

    Token &Parser::peek() {
        return tokens[current];
    }

    Stmt *Parser::statement() {
        if (match(std::vector<TokenType>{TokenType::FOR}))
            return forStatement();
        if (match(std::vector<TokenType>{TokenType::IF}))
            return ifStatement();
        if (match(std::vector<TokenType>{TokenType::PRINT}))
            return printStatement();
        if (match(std::vector<TokenType>{TokenType::RETURN}))
            return returnStatement();
        if (match(std::vector<TokenType>{TokenType::WHILE}))
            return whileStatement();
        if (match(std::vector<TokenType>{TokenType::LEFT_BRACE}))
            return new Block(block());

        return expressionStatement();
    }

    void Parser::synchronize() {
        advance();

        while (!isAtEnd()) {
            if (previous().type == TokenType::SEMICOLON) return;

            switch (peek().type) {
                case TokenType::CLASS:
                case TokenType::FUN:
                case TokenType::VAR:
                case TokenType::FOR:
                case TokenType::IF:
                case TokenType::WHILE:
                case TokenType::PRINT:
                case TokenType::RETURN:
                    return;
            }

            advance();
        }
    }

    Token &Parser::advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    Token &Parser::previous() {
        return tokens[current - 1];
    }

    bool Parser::match(const std::vector<TokenType> &types) {
        for (TokenType type: types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

    bool Parser::check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    Stmt *Parser::classDeclaration() {
        Token &name = consume(TokenType::IDENTIFIER, "Expect class name.");

        Variable *superclass = nullptr;
        if (match(std::vector<TokenType>{TokenType::LESS})) {
            consume(TokenType::IDENTIFIER, "Expect superclass name.");
            superclass = new Variable(previous());
        }

        consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");

        std::vector<Function *> methods;
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            methods.push_back(function("method"));
        }

        consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");

        return new Class(name, superclass, methods);
    }

    Token &Parser::consume(TokenType type, const std::string& message) {
        if (check(type)) return advance();
        throw error(peek(), message);
    }

    ParseError Parser::error(Token &token, const std::string& message) {
        cpplox::error(token, message);
        return ParseError{};
    }

    Function *Parser::function(const std::string& kind) {
        Token &name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");

        consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
        auto *parameters = new std::vector<Token>();

        if (!check(TokenType::RIGHT_PAREN)) {
            do {
                if (parameters->size() >= 255) {
                    error(peek(), "Can't have more than 255 parameters.");
                }

                parameters->push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
            } while (match(std::vector<TokenType>{TokenType::COMMA}));
        }
        consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

        consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
        std::vector<Stmt *> body = block();
        return new Function(name, parameters, body);
    }

    std::vector<Stmt *> Parser::block() {
        std::vector<Stmt *> statements;

        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            statements.push_back(declaration());
        }

        consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
        return std::move(statements);
    }

    Stmt *Parser::varDeclaration() {
        Token &name = consume(TokenType::IDENTIFIER, "Expect variable name.");

        Expr *initializer = nullptr;
        if (match(std::vector<TokenType>{TokenType::EQUAL})) {
            initializer = expression();
        }
        consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
        return new Var(name, initializer);
    }

    Expr *Parser::expression() {
        return assignment();
    }

    Expr *Parser::assignment() {
        Expr *expr = _or();

        if (match(std::vector<TokenType>{TokenType::EQUAL})) {
            Token &equals = previous();
            Expr *value = assignment();

            auto *variable = dynamic_cast<Variable *>(expr);
            auto *get = dynamic_cast<Get *>(expr);
            if (variable != nullptr) {
                Token& name = variable->name;
                return new Assign(name, value);
            } else if (get != nullptr) {
                return new Set(get->object, get->name, value);
            }
            error(equals, "Invalid assignment target.");
        }

        return expr;
    }

    Expr *Parser::_or() {
        Expr *expr = _and();

        while (match(std::vector<TokenType>{TokenType::OR})) {
            Token &op = previous();
            Expr *right = _and();
            expr = new Logical(expr, op, right);
        }

        return expr;
    }

    Expr *Parser::_and() {
        Expr *expr = equality();

        while (match(std::vector<TokenType>{TokenType::AND})) {
            Token &op = previous();
            Expr *right = equality();
            expr = new Logical(expr, op, right);
        }

        return expr;
    }

    Expr *Parser::equality() {
        Expr *expr = comparison();

        while (match(std::vector<TokenType>{TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
            Token &op = previous();
            Expr *right = comparison();
            expr = new Binary(expr, op, right);
        }

        return expr;
    }

    Expr *Parser::comparison() {
        Expr *expr = term();

        while (match(std::vector<TokenType>
                             {TokenType::GREATER,
                              TokenType::GREATER_EQUAL,
                              TokenType::LESS,
                              TokenType::LESS_EQUAL})) {
            Token &op = previous();
            Expr *right = term();
            expr = new Binary(expr, op, right);
        }

        return expr;
    }

    Expr *Parser::term() {
        Expr *expr = factor();

        while (match(std::vector<TokenType>{TokenType::MINUS, TokenType::PLUS})) {
            Token &op = previous();
            Expr *right = factor();
            expr = new Binary(expr, op, right);
        }

        return expr;
    }

    Expr *Parser::factor() {
        Expr *expr = unary();

        while (match(std::vector<TokenType>{TokenType::SLASH, TokenType::STAR})) {
            Token &op = previous();
            Expr *right = unary();
            expr = new Binary(expr, op, right);
        }

        return expr;
    }

    Expr *Parser::unary() {
        if (match(std::vector<TokenType>{TokenType::BANG, TokenType::MINUS})) {
            Token &op = previous();
            Expr *right = unary();
            return new Unary(op, right);
        }

        return call();
    }

    Expr *Parser::call() {
        Expr *expr = primary();

        while (true) {
            if (match(std::vector<TokenType>{TokenType::LEFT_PAREN})) {
                expr = finishCall(expr);
            } else if (match(std::vector<TokenType>{TokenType::DOT})) {
                Token &name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
                expr = new Get(expr, name);
            } else {
                break;
            }
        }

        return expr;
    }

    Expr *Parser::finishCall(Expr *callee) {
        std::vector<Expr *> arguments;

        if (!check(TokenType::RIGHT_PAREN)) {
            do {
                if (arguments.size() >= 255) {
                    error(peek(), "Can't have more than 255 arguments.");
                }
                arguments.push_back(expression());
            } while (match(std::vector<TokenType>{TokenType::COMMA}));
        }

        Token &paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

        return new Call(callee, paren, std::move(arguments));
    }

    Expr *Parser::primary() {
        if (match(std::vector<TokenType>{TokenType::FALSE}))
            return new Literal(new Boolean(false));
        if (match(std::vector<TokenType>{TokenType::TRUE}))
            return new Literal(new Boolean(true));
        if (match(std::vector<TokenType>{TokenType::NIL}))
            return new Literal(nullptr);

        if (match(std::vector<TokenType>{TokenType::NUMBER, TokenType::STRING})) {
            return new Literal(previous().literal->clone());
        }

        if (match(std::vector<TokenType>{TokenType::SUPER})) {
            Token &keyword = previous();
            consume(TokenType::DOT, "Expect '.' after 'super'.");
            Token &method = consume(TokenType::IDENTIFIER, "Expect superclass method name.");
            return new Super(keyword, method);
        }

        if (match(std::vector<TokenType>{TokenType::THIS}))
            return new This(previous());

        if (match(std::vector<TokenType>{TokenType::IDENTIFIER})) {
            return new Variable(previous());
        }

        if (match(std::vector<TokenType>{TokenType::LEFT_PAREN})) {
            Expr *expr = expression();
            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
            return new Grouping(expr);
        }

        throw error(peek(), "Expect expression.");
    }

    Stmt *Parser::forStatement() {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

        Stmt *initializer = nullptr;
        if (match(std::vector<TokenType>{TokenType::SEMICOLON})) {
            initializer = nullptr;
        } else if (match(std::vector<TokenType>{TokenType::VAR})) {
            initializer = varDeclaration();
        } else {
            initializer = expressionStatement();
        }

        Expr *condition = nullptr;
        if (!check(TokenType::SEMICOLON)) {
            condition = expression();
        }
        consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

        Expr *increment = nullptr;
        if (!check(TokenType::RIGHT_PAREN)) {
            increment = expression();
        }
        consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

        Stmt *body = statement();

        if (increment != nullptr) {
            body = new Block(std::vector<Stmt *>{body, new Expression(increment)});
        }

        if (condition == nullptr) condition = new Literal(new Boolean(true));
        body = new While(condition, body);

        if (initializer != nullptr) {
            body = new Block(std::vector<Stmt *>{initializer, body});
        }

        return body;
    }

    Stmt *Parser::expressionStatement() {
        Expr *expr = expression();

        consume(TokenType::SEMICOLON, "Expect ';' after expression.");
        return new Expression(expr);
    }

    Stmt *Parser::ifStatement() {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
        Expr *condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

        Stmt *thenBranch = statement();
        Stmt *elseBranch = nullptr;
        if (match(std::vector<TokenType>{TokenType::ELSE})) {
            elseBranch = statement();
        }

        return new If(condition, thenBranch, elseBranch);
    }

    Stmt *Parser::printStatement() {
        Expr *value = expression();

        consume(TokenType::SEMICOLON, "Expect ';' after value.");
        return new Print(value);
    }

    Stmt *Parser::returnStatement() {
        Token &keyword = previous();

        Expr *value = nullptr;
        if (!check(TokenType::SEMICOLON)) {
            value = expression();
        }

        consume(TokenType::SEMICOLON, "Expect ';' after return value.");
        return new Return(keyword, value);
    }

    Stmt *Parser::whileStatement() {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
        Expr *condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
        Stmt *body = statement();

        return new While(condition, body);
    }

    Parser::~Parser() = default;
}
