//
// Created by hlx on 2023/9/7.
//

#ifndef CPPLOX_STMT_H
#define CPPLOX_STMT_H

#include <utility>
#include "token.h"
#include "object.h"
#include "expr.h"

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

    namespace stmt {
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
    }


    class Stmt {
    public:
        virtual Object *accept(stmt::Visitor *visitor) = 0;

        virtual ~Stmt() = default;
    };


    class Block : public Stmt {
    public:
        explicit Block(std::vector<Stmt *> statements) {
            this->statements = std::move(statements);
        }

        Object *accept(stmt::Visitor *visitor) override {
            return visitor->visitBlockStmt(this);
        }

        ~Block() override {
            for (auto statement: statements) {
                delete statement;
            }
        };
        std::vector<Stmt *> statements;
    };


    class Expression : public Stmt {
    public:
        explicit Expression(Expr *expression) {
            this->expression = expression;
        }

        Object *accept(stmt::Visitor *visitor) override {
            return visitor->visitExpressionStmt(this);
        }

        ~Expression() override {
            delete expression;
        };
        Expr *expression;
    };


    class Function : public Stmt {
    public:
        Function(Token &name, std::vector<Token> *params, std::vector<Stmt *> body) {
            this->name = name;
            this->params = params;
            this->body = std::move(body);
        }

        Object *accept(stmt::Visitor *visitor) override {
            return visitor->visitFunctionStmt(this);
        }

        ~Function() override {
            delete params;
            for (auto statement: body) {
                delete statement;
            }
        };
        std::vector<Stmt *> body;
        std::vector<Token> *params;
        Token name;
    };

    class Class : public Stmt {
    public:
        Class(Token &name, Variable *superclass, std::vector<Function *> methods) {
            this->name = name;
            this->superclass = superclass;
            this->methods = std::move(methods);
        }

        Object *accept(stmt::Visitor *visitor) override {
            return visitor->visitClassStmt(this);
        }

        ~Class() override {
            delete superclass;
            for (auto method: methods) {
                delete method;
            }
        };
        Token name;
        Variable *superclass;
        std::vector<Function *> methods;
    };

    class If : public Stmt {
    public:
        If(Expr *condition, Stmt *thenBranch, Stmt *elseBranch) {
            this->condition = condition;
            this->thenBranch = thenBranch;
            this->elseBranch = elseBranch;
        }

        Object *accept(stmt::Visitor *visitor) override {
            return visitor->visitIfStmt(this);
        }

        ~If() override {
            delete condition;
            delete thenBranch;
            delete elseBranch;
        };
        Expr *condition;
        Stmt *thenBranch;
        Stmt *elseBranch;
    };


    class Print : public Stmt {
    public:
        explicit Print(Expr *expression) {
            this->expression = expression;
        }

        Object *accept(stmt::Visitor *visitor) override {
            return visitor->visitPrintStmt(this);
        }

        ~Print() override {
            delete expression;
        };
        Expr *expression;
    };

    class Return : public Stmt {
    public:
        Return(Token &keyword, Expr *value) {
            this->keyword = keyword;
            this->value = value;
        }

        Object *accept(stmt::Visitor *visitor) override {
            return visitor->visitReturnStmt(this);
        }

        ~Return() override {
            delete value;
        };
        Token keyword;
        Expr *value;
    };

    class Var : public Stmt {
    public:
        Var(Token &name, Expr *initializer) {
            this->name = name;
            this->initializer = initializer;
        }

        Object *accept(stmt::Visitor *visitor) override {
            return visitor->visitVarStmt(this);
        }

        ~Var() override {
            delete initializer;
        };
        Token name;
        Expr *initializer;
    };


    class While : public Stmt {
    public:
        While(Expr *condition, Stmt *body) {
            this->condition = condition;
            this->body = body;
        }

        Object *accept(stmt::Visitor *visitor) override {
            return visitor->visitWhileStmt(this);
        }

        ~While() override {
            delete condition;
            delete body;
        };
        Expr *condition;
        Stmt *body;
    };

}

#endif //CPPLOX_STMT_H
