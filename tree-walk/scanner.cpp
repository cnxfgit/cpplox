//
// Created by hlx on 2023/9/6.
//
#include <map>
#include "scanner.h"

namespace cpplox {

    std::map<std::string, TokenType> keywords = {
            {"and",    TokenType::AND},
            {"class",  TokenType::CLASS},
            {"else",   TokenType::ELSE},
            {"false",  TokenType::FALSE},
            {"for",    TokenType::FOR},
            {"fun",    TokenType::FUN},
            {"if",     TokenType::IF},
            {"nil",    TokenType::NIL},
            {"or",     TokenType::OR},
            {"print",  TokenType::PRINT},
            {"return", TokenType::RETURN},
            {"super",  TokenType::SUPER},
            {"this",   TokenType::THIS},
            {"true",   TokenType::TRUE},
            {"var",    TokenType::VAR},
            {"while",  TokenType::WHILE},
    };

    void error(int line, const std::string &message);

    bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    bool isAlpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    bool isAlphaNumeric(char c) {
        return isAlpha(c) || isDigit(c);
    }

    Scanner::Scanner(std::string source) {
        this->source = std::move(source);
        this->tokens = std::vector<Token>();
        this->start = 0;
        this->current = 0;
        this->line = 1;
    }

    std::vector<Token> Scanner::scanTokens() {
        while (!isAtEnd()) {
            // We are at the beginning of the next lexeme.
            start = current;
            scanToken();
        }
        tokens.emplace_back(TokenType::END, "", nullptr, line);
        return std::move(tokens);
    }

    bool Scanner::isAtEnd() {
        return current >= source.length();
    }

    void Scanner::scanToken() {
        char c = advance();
        switch (c) {
            case '(':
                addToken(TokenType::LEFT_PAREN);
                break;
            case ')':
                addToken(TokenType::RIGHT_PAREN);
                break;
            case '{':
                addToken(TokenType::LEFT_BRACE);
                break;
            case '}':
                addToken(TokenType::RIGHT_BRACE);
                break;
            case ',':
                addToken(TokenType::COMMA);
                break;
            case '.':
                addToken(TokenType::DOT);
                break;
            case '-':
                addToken(TokenType::MINUS);
                break;
            case '+':
                addToken(TokenType::PLUS);
                break;
            case ';':
                addToken(TokenType::SEMICOLON);
                break;
            case '*':
                addToken(TokenType::STAR);
                break;
            case '!':
                addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
                break;
            case '=':
                addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
                break;
            case '<':
                addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
                break;
            case '>':
                addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
                break;
            case '/':
                if (match('/')) {
                    // A comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    addToken(TokenType::SLASH);
                }
                break;
            case ' ':
            case '\r':
            case '\t':
                // Ignore whitespace.
                break;
            case '\n':
                line++;
                break;
            case '"':
                string();
                break;
            default:
                if (isDigit(c)) {
                    number();
                } else if (isAlpha(c)) {
                    identifier();
                } else {
                    error(line, "Unexpected character.");
                }
                break;
        }
    }

    char Scanner::advance() {
        return source[current++];
    }

    void Scanner::addToken(TokenType type) {
        addToken(type, nullptr);
    }

    void Scanner::addToken(TokenType type, Object *literal) {
        std::string text = source.substr(start, current - start);
        tokens.emplace_back(type, text, literal, line);
    }

    bool Scanner::match(char expected) {
        if (isAtEnd()) return false;
        if (source[current] != expected) return false;

        current++;
        return true;
    }

    char Scanner::peek() {
        if (isAtEnd()) return '\0';
        return source[current];
    }

    void Scanner::string() {
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') line++;
            advance();
        }

        if (isAtEnd()) {
            error(line, "Unterminated string.");
            return;
        }

        // The closing .
        advance();

        // Trim the surrounding quotes.
        std::string value = source.substr(start + 1, (current - 1) - (start + 1));
        addToken(TokenType::STRING, new String(value));
    }

    void Scanner::number() {
        while (isDigit(peek())) advance();

        // Look for a fractional part.
        if (peek() == '.' && isDigit(peekNext())) {
            // Consume the "."
            advance();

            while (isDigit(peek())) advance();
        }

        addToken(TokenType::NUMBER,
                 new Number(std::stod(source.substr(start, current - start))));
    }

    char Scanner::peekNext() {
        if (current + 1 >= source.length()) return '\0';
        return source[current + 1];
    }

    void Scanner::identifier() {
        while (isAlphaNumeric(peek())) advance();

        std::string text = source.substr(start, current - start);
        TokenType type;
        if (keywords.find(text) != keywords.end()) {
            type = keywords[text];
        } else {
            type = TokenType::IDENTIFIER;
        }

        addToken(type);
    }

    Scanner::~Scanner() =
    default;

}


