//
// Created by hlx on 2023/9/7.
//

#include <vector>
#include "expr.h"

namespace cpplox {

    class Assign : public Expr {
    private:
        Token name;
        Expr *value;
    public:
        Assign(Token name, Expr *value) {
            this->name = std::move(name);
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
        Binary(Expr *left, Token op, Expr *right) {
            this->left = left;
            this->op = std::move(op);
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
        std::vector<Expr> arguments;
    public:
        Call(Expr *callee, Token paren, std::vector<Expr> arguments) {
            this->callee = callee;
            this->paren = std::move(paren);
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
    private:
        Expr *object;
        Token name;
    public:
        Get(Expr *object, Token name) {
            this->object = object;
            this->name = std::move(name);
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitGetExpr(this);
        }

        ~Get() override {
            delete object;
        }
    };

    class Grouping : public Expr {
    private:
        Expr *object;
        Token name;
    public:
        Grouping(Expr *object, Token name) {
            this->object = object;
            this->name = std::move(name);
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitGroupingExpr(this);
        }

        ~Grouping() override {
            delete object;
        }
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
        Logical(Expr *left, Token op, Expr *right) {
            this->left = left;
            this->op = std::move(op);
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
        Super(Token keyword, Token method) {
            this->keyword = std::move(keyword);
            this->method = std::move(method);
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
        Unary(Token op, Expr *right) {
            this->op = std::move(op);
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
    private:
        Token op;
        Expr *right;
    public:
        Variable(Token op, Expr *right) {
            this->op = std::move(op);
            this->right = right;
        }

        Object *accept(expr::Visitor *visitor) override {
            return visitor->visitVariableExpr(this);
        }

        ~Variable() override {
            delete right;
        };
    };

}
