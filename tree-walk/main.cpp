//
// Created by hlx on 2023/9/6.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include "scanner.h"

namespace cpplox {
    bool hadError = false;
    bool hadRuntimeError = false;

    void run(std::string source) {
        Scanner scanner(std::move(source));
        std::vector<Token> &tokens = scanner.scanTokens();

    }

    void runFile(const char *path) {
        std::ifstream ifs(path);
        std::stringstream buffer;
        buffer << ifs.rdbuf();

        run(buffer.str());

        if (hadError) exit(65);
        if (hadRuntimeError) exit(70);
    }

    void runPrompt() {

    }

    void report(int line, const std::string &where, const std::string &message) {
        std::cerr << "[line " + std::to_string(line) + "] Error" + where + ": " + message << std::endl;
        hadError = true;
    }

    void error(int line, const std::string &message) {
        report(line, "", message);
    }

}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        std::cerr << "Usage: cpplox [script]" << std::endl;
        exit(64);
    } else if (argc == 2) {
        cpplox::runFile(argv[1]);
    } else {
        cpplox::runPrompt();
    }
    return 0;
}

