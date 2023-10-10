//
// Created by hlx on 2023/9/6.
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

#include "chunk.h"
#include "vm.h"

namespace cpplox{
    // 命令模式 最长为1024
    static void repl() {
        char line[1024];
        for (;;) {
            printf("> ");

            if (!fgets(line, sizeof(line), stdin)) {
                printf("\n");
                break;
            }

            vm.interpret(line);
        }
    }

    // 读取文件内容
    static std::string readFile(const char* path) {
        std::ifstream ifs(path);
        std::stringstream buffer;
        buffer << ifs.rdbuf();

        return buffer.str();
    }

    // 用传入的文件路径读取文件 并解释执行
    static void runFile(const char* path) {
        std::string source = readFile(path);
        InterpretResult result = vm.interpret(source.c_str());

        if (result == InterpretResult::COMPILE_ERROR) exit(65);
        if (result == InterpretResult::RUNTIME_ERROR) exit(70);
    }
}



int main(int argc, const char *argv[]) {
    cpplox::initVM();

    // 启动参数校验  一个参数为指令模式  两个参数为文件模式
    if (argc == 1) {
        cpplox::repl(); // 指令模式
    } else if (argc == 2) {
        cpplox::runFile(argv[1]);   // 文件模式
    } else {
        fprintf(stderr, "Usage: cpplox [path]\n");
        exit(64);
    }

    cpplox::freeVM();
    return 0;
}
