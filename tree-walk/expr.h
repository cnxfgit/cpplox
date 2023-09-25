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

    class Assign : public Expr {
    private:
        Token name;
        Expr *value;
    public:
        Assign(Token& name, Expr *value) {
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
    private:
        Expr *left;
        Token op;
        Expr *right;

    public:
        Binary(Expr *left, Token& op, Expr *right) {
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
    private:
        Expr *callee;
        Token paren;
        std::vector<Expr *> arguments;
    public:
        Call(Expr *callee, Token& paren, std::vector<Expr *> arguments) {
            this->callee = callee;
            this->paren = paren;
            this->arguments = std::move(arguments);
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitCallExpr(this);
        }

        ~Call() override {
            delete callee;
        }
    };

    class Get : public Expr {
    public:
        Get(Expr *object, Token& name) {
            this->object = object;
            this->name = name;
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitGetExpr(this);
        }

        ~Get() override {
            delete object;
        }

        Expr *object;
        Token name;
    };

    class Grouping : public Expr {
    private:
        Expr* expression;
    public:
        explicit Grouping(Expr* expression) {
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
    private:
        Object *value;
    public:
        explicit Literal(Object *value) {
            this->value = value;
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitLiteralExpr(this);
        }

        ~Literal() override {
            delete value;
        }
    };

    class Logical : public Expr {
    private:
        Expr *left;
        Token op;
        Expr *right;
    public:
        Logical(Expr *left, Token& op, Expr *right) {
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
    private:
        Expr *object;
        Token name;
        Expr *value;
    public:
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
    private:
        Token keyword;
        Token method;
    public:
        Super(Token& keyword, Token& method) {
            this->keyword = keyword;
            this->method = method;
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitSuperExpr(this);
        }

        ~Super() override = default;
    };

    class This : public Expr {
    private:
        Token keyword;
    public:
        explicit This(Token keyword) {
            this->keyword = std::move(keyword);
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitThisExpr(this);
        }

        ~This() override = default;
    };

    class Unary : public Expr {
    private:
        Token op;
        Expr *right;
    public:
        Unary(Token& op, Expr *right) {
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
        explicit Variable(Token &name) {
            this->name = name;
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitVariableExpr(this);
        }

        ~Variable() override = default;

        Token name;
    };

}

#endif //CPPLOX_EXPR_H
