//
// Created by hlx on 2023/9/7.
//

#include <vector>
#include "stmt.h"
#include "expr.h"


namespace cpplox {
    class Block : public Stmt {
    private:
        std::vector<Stmt> statements;
    public:
        explicit Block(std::vector<Stmt> statements) {
            this->statements = std::move(statements);
        }

        Object *accept(Visitor *visitor) override {
            return visitor->visitBlockStmt(this);
        }

        ~Block() override = default;
    };

    class Class : public Stmt {
    private:
        Token name;
        Variable *superclass;
        std::vector<Function> methods;
    public:
        Class(Token name, Variable *superclass, std::vector<Function> methods) {
            this->name = std::move(name);
            this->superclass = superclass;
            this->methods = std::move(methods);
        }

        Object *accept(Visitor *visitor) override {
            return visitor->visitClassStmt(this);
        }

        ~Class() override {
            delete superclass;
        };
    };

    class Expression : public Stmt {
    private:
        Expr *expression;
    public:
        explicit Expression(Expr *expression) {
            this->expression = expression;
        }

        Object *accept(Visitor *visitor) override {
            return visitor->visitExpressionStmt(this);
        }

        ~Expression() override {
            delete expression;
        };
    };


    class Function : public Stmt {
    private:
        Token name;
        std::vector<Token> params;
        std::vector<Stmt> body;
    public:
        Function(Token name, std::vector<Token> params, std::vector<Stmt> body) {
            this->name = std::move(name);
            this->params = std::move(params);
            this->body = std::move(body);
        }

        Object *accept(Visitor *visitor) override {
            return visitor->visitFunctionStmt(this);
        }

        ~Function() override = default;
    };


    class If : public Stmt {
    private:
        Expr *condition;
        Stmt *thenBranch;
        Stmt *elseBranch;
    public:
        If(Expr *condition, Stmt *thenBranch, Stmt *elseBranch) {
            this->condition = condition;
            this->thenBranch = thenBranch;
            this->elseBranch = elseBranch;
        }

        Object *accept(Visitor *visitor) override {
            return visitor->visitIfStmt(this);
        }

        ~If() override {
            delete condition;
            delete thenBranch;
            delete elseBranch;
        };
    };


    class Print : public Stmt {
    private:
        Expr *expression;
    public:
        explicit Print(Expr *expression) {
            this->expression = expression;
        }

        Object *accept(Visitor *visitor) override {
            return visitor->visitPrintStmt(this);
        }

        ~Print() override {
            delete expression;
        };
    };

    class Return : public Stmt {
    private:
        Token keyword;
        Expr *value;
    public:
        Return(Token keyword, Expr *value) {
            this->keyword = std::move(keyword);
            this->value = value;
        }

        Object *accept(Visitor *visitor) override {
            return visitor->visitReturnStmt(this);
        }

        ~Return() override {
            delete value;
        };
    };

    class Var : public Stmt {
    private:
        Token name;
        Expr *initializer;
    public:
        Var(Token name, Expr *initializer) {
            this->name = std::move(name);
            this->initializer = initializer;
        }

        Object *accept(Visitor *visitor) override {
            return visitor->visitVarStmt(this);
        }

        ~Var() override {
            delete initializer;
        };
    };


    class While : public Stmt {
    private:
        Expr *condition;
        Stmt *body;
    public:
        While(Expr *condition, Stmt *body) {
            this->condition = condition;
            this->body = body;
        }

        Object *accept(Visitor *visitor) override {
            return visitor->visitWhileStmt(this);
        }

        ~While() override {
            delete condition;
            delete body;
        };
    };
}