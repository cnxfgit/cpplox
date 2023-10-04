//
// Created by hlx on 2023/9/7.
//

#ifndef CPPLOX_EXPR_H
#define CPPLOX_EXPR_H

#include "token.h"
#include <vector>

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

    class Assign : public Expr {
    public:
        Token name;
        Expr *value;

        Assign(Token &name, Expr *value) {
            this->name = name;
            this->value = value;
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitAssignExpr(this);
        }

        ~Assign() override {
            delete value;
        }
    };

    class Binary : public Expr {

    public:
        Expr *left;
        Expr *right;
        Token op;

        Binary(Expr *left, Token &op, Expr *right) {
            this->left = left;
            this->op = op;
            this->right = right;
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitBinaryExpr(this);
        }

        ~Binary() override {
            delete left;
            delete right;
        }
    };

    class Call : public Expr {
    public:
        std::vector<Expr *> arguments;
        Expr *callee;
        Token paren;

        Call(Expr *callee, Token &paren, std::vector<Expr *> arguments) {
            this->callee = callee;
            this->paren = paren;
            this->arguments = std::move(arguments);
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitCallExpr(this);
        }

        ~Call() override {
            delete callee;
            for (auto argument: arguments) {
                delete argument;
            }
        }
    };

    class Get : public Expr {
    public:
        Expr *object;
        Token name;

        Get(Expr *object, Token &name) {
            this->object = object;
            this->name = name;
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitGetExpr(this);
        }

        ~Get() override {
            delete object;
        }
    };

    class Grouping : public Expr {
    public:
        Expr *expression;

        explicit Grouping(Expr *expression) {
            this->expression = expression;
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitGroupingExpr(this);
        }

        ~Grouping() override {
            delete expression;
        };
    };

    class Literal : public Expr {
    public:
        Object *value;

        explicit Literal(Object *value) {
            this->value = value;
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitLiteralExpr(this);
        }

        ~Literal() override {
            //delete value;
        }
    };

    class Logical : public Expr {
    public:
        Expr *left;
        Expr *right;
        Token op;

        Logical(Expr *left, Token &op, Expr *right) {
            this->left = left;
            this->op = op;
            this->right = right;
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitLogicalExpr(this);
        }

        ~Logical() override {
            delete left;
            delete right;
        }
    };

    class Set : public Expr {
    public:
        Expr *value;
        Expr *object;
        Token name;

        Set(Expr *object, Token name, Expr *value) {
            this->object = object;
            this->name = std::move(name);
            this->value = value;
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitSetExpr(this);
        }

        ~Set() override {
            delete object;
            delete value;
        }
    };

    class Super : public Expr {
    public:
        Token keyword;
        Token method;

        Super(Token &keyword, Token &method) {
            this->keyword = keyword;
            this->method = method;
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitSuperExpr(this);
        }

        ~Super() override = default;
    };

    class This : public Expr {
    public:
        Token keyword;

        explicit This(Token& keyword) {
            this->keyword = keyword;
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitThisExpr(this);
        }

        ~This() override = default;
    };

    class Unary : public Expr {
    public:
        Expr *right;
        Token op;

        Unary(Token &op, Expr *right) {
            this->op = op;
            this->right = right;
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitUnaryExpr(this);
        }

        ~Unary() override {
            delete right;
        };
    };

    class Variable : public Expr {
    public:
        Token name;

        explicit Variable(Token &name) {
            this->name = name;
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitVariableExpr(this);
        }

        ~Variable() override = default;
    };

}

#endif //CPPLOX_EXPR_H
