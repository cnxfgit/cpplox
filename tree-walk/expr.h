//
// Created by hlx on 2023/9/7.
//

#ifndef CPPLOX_EXPR_H
#define CPPLOX_EXPR_H

#include "token.h"

namespace cpplox {

    class Assign;

    class Binary;

    class Call;

    class Get;

    class Grouping;

    class Literal;

    class Logical;

    class Set;

    class Super;

    class This;

    class Unary;

    class Variable;

    namespace expr {
        class Visitor {
        public:
            virtual Object *visitAssignExpr(Assign *expr) = 0;

            virtual Object *visitBinaryExpr(Binary *expr) = 0;

            virtual Object *visitCallExpr(Call *expr) = 0;

            virtual Object *visitGetExpr(Get *expr) = 0;

            virtual Object *visitGroupingExpr(Grouping *expr) = 0;

            virtual Object *visitLiteralExpr(Literal *expr) = 0;

            virtual Object *visitLogicalExpr(Logical *expr) = 0;

            virtual Object *visitSetExpr(Set *expr) = 0;

            virtual Object *visitSuperExpr(Super *expr) = 0;

            virtual Object *visitThisExpr(This *expr) = 0;

            virtual Object *visitUnaryExpr(Unary *expr) = 0;

            virtual Object *visitVariableExpr(Variable *expr) = 0;
        };
    }

    class Expr {
    public:
        virtual Object *accept(expr::Visitor *visitor) = 0;

        virtual ~Expr() = default;
    };

}

#endif //CPPLOX_EXPR_H
