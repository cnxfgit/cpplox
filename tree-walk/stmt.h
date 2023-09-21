//
// Created by hlx on 2023/9/7.
//

#ifndef CPPLOX_STMT_H
#define CPPLOX_STMT_H

#include <utility>
#include "token.h"
#include "object.h"

namespace cpplox {


    class Block;

    class Class;

    class Expression;

    class Function;

    class If;

    class Print;

    class Return;

    class Var;

    class While;

    class Visitor {
    public:
        virtual Object *visitBlockStmt(Block *stmt) = 0;

        virtual Object *visitClassStmt(Class *stmt) = 0;

        virtual Object *visitExpressionStmt(Expression *stmt) = 0;

        virtual Object *visitFunctionStmt(Function *stmt) = 0;

        virtual Object *visitIfStmt(If *stmt) = 0;

        virtual Object *visitPrintStmt(Print *stmt) = 0;

        virtual Object *visitReturnStmt(Return *stmt) = 0;

        virtual Object *visitVarStmt(Var *stmt) = 0;

        virtual Object *visitWhileStmt(While *stmt) = 0;
    };

    class Stmt {
    public:
        virtual Object *accept(Visitor *visitor) = 0;

        virtual ~Stmt() = default;
    };

}

#endif //CPPLOX_STMT_H
