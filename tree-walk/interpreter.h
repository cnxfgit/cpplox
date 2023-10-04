//
// Created by hlx on 2023/9/27.
//

#ifndef CPPLOX_INTERPRETER_H
#define CPPLOX_INTERPRETER_H

#include <utility>

#include "expr.h"
#include "environment.h"
#include "stmt.h"

namespace cpplox {

    class RuntimeError : public std::exception {
    public:
        Token token;
        std::string message;

        RuntimeError(Token &token, std::string message) noexcept {
            this->message = std::move(message);
            this->token = token;
        }

        const char *what() const noexcept override {
            return this->message.c_str();
        }

        ~RuntimeError() override = default;
    };

    class Interpreter : public expr::Visitor, public stmt::Visitor {
    private:

        Environment *environment;
        std::map<Expr *, int> locals;

    public:
        Environment *globals;

        Interpreter();

        void interpret(std::vector<Stmt *> &statements);

        void resolve(Expr *expr, int depth);

        void executeBlock(const std::vector<Stmt *> &statements, Environment *environment);

        Object *visitBlockStmt(Block *stmt) override;

        Object *visitClassStmt(Class *stmt) override;

        Object *visitExpressionStmt(Expression *stmt) override;

        Object *visitFunctionStmt(Function *stmt) override;

        Object *visitIfStmt(If *stmt) override;

        Object *visitPrintStmt(Print *stmt) override;

        Object *visitReturnStmt(Return *stmt) override;

        Object *visitVarStmt(Var *stmt) override;

        Object *visitAssignExpr(Assign *expr) override;

        Object *visitBinaryExpr(Binary *expr) override;

        Object *visitCallExpr(Call *expr) override;

        Object *visitGetExpr(Get *expr) override;

        Object *visitGroupingExpr(Grouping *expr) override;

        Object *visitLiteralExpr(Literal *expr) override;

        Object *visitLogicalExpr(Logical *expr) override;

        Object *visitSetExpr(Set *expr) override;

        Object *visitSuperExpr(Super *expr) override;

        Object *visitThisExpr(This *expr) override;

        Object *visitUnaryExpr(Unary *expr) override;

        Object *visitVariableExpr(Variable *expr) override;

        Object *visitWhileStmt(While *stmt) override;

        void removeVariable(const std::string& name);

        ~Interpreter();

    private:
        void execute(Stmt *stmt);

        Object *evaluate(Expr *expr);

        Object *lookUpVariable(Token &name, Expr *expr);
    };

    class ReturnException : public std::exception {
    public:
        Object *value;

        explicit ReturnException(Object *value) {
            this->value = value;
        };
    };
}

#endif //CPPLOX_INTERPRETER_H
