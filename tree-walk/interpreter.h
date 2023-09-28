//
// Created by hlx on 2023/9/27.
//

#ifndef CPPLOX_INTERPRETER_H
#define CPPLOX_INTERPRETER_H

#include "expr.h"

namespace cpplox {
    class Interpreter {
    public:
        void resolve(Expr* expr, int depth);
    };
}

#endif //CPPLOX_INTERPRETER_H
