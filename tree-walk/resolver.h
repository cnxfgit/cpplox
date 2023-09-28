//
// Created by hlx on 2023/9/25.
//

#ifndef CPPLOX_RESOLVER_H
#define CPPLOX_RESOLVER_H

#include <deque>
#include <map>
#include "stmt.h"
#include "expr.h"
#include "interpreter.h"

namespace cpplox {

    enum class FunctionType {
        NONE,
        FUNCTION,
        INITIALIZER,
        METHOD
    };

    enum class ClassType {
        NONE,
        CLASS,
        SUBCLASS
    };

    class Resolver : public expr::Visitor, public stmt::Visitor {
    private:
        Interpreter interpreter;
        std::deque<std::map<std::string, bool>> scopes;
        FunctionType currentFunction = FunctionType::NONE;
        ClassType currentClass = ClassType::NONE;
    public:
        void resolve(const std::vector<Stmt *> &statements);

    private:
        void resolve(Stmt *stmt);

        void resolve(Expr *expr);

        void resolveFunction(Function *function, FunctionType type);

        void resolveLocal(Expr *expr, Token &name);

        void beginScope();

        void endScope();

        void declare(Token &name);

        void define(Token &name);

        Object *visitBlockStmt(Block *stmt) override;

        Object *visitClassStmt(Class *stmt) override;

        Object *visitExpressionStmt(Expression *stmt) override;

        Object *visitFunctionStmt(Function *stmt) override;

        Object *visitIfStmt(If *stmt) override;

        Object *visitPrintStmt(Print *stmt) override;

        Object *visitReturnStmt(Return *stmt) override;

        Object *visitVarStmt(Var *stmt) override;

        Object *visitWhileStmt(While *stmt) override;

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
    };

}

#endif //CPPLOX_RESOLVER_H
