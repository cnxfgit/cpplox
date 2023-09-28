//
// Created by hlx on 2023/9/25.
//

#include "resolver.h"

namespace cpplox {

    void error(Token &token, const std::string &message);

    void Resolver::resolve(const std::vector<Stmt *> &statements) {
        for (Stmt *statement: statements) {
            resolve(statement);
        }
    }

    void Resolver::resolve(Stmt *stmt) {
        stmt->accept(this);
    }

    void Resolver::resolve(Expr *expr) {
        expr->accept(this);
    }

    void Resolver::resolveFunction(Function *function, FunctionType type) {
        FunctionType enclosingFunction = currentFunction;
        currentFunction = type;

        beginScope();
        for (Token &param: *function->params) {
            declare(param);
            define(param);
        }
        resolve(function->body);
        endScope();

        currentFunction = enclosingFunction;
    }

    void Resolver::resolveLocal(Expr *expr, Token &name) {
        for (int i = (int) scopes.size() - 1; i >= 0; i--) {
            if (scopes[i].find(name.lexeme) != scopes[i].end()) {
                interpreter.resolve(expr, (int) scopes.size() - 1 - i);
                return;
            }
        }
    }

    void Resolver::beginScope() {
        scopes.emplace_back();
    }

    void Resolver::endScope() {
        scopes.pop_back();
    }

    void Resolver::declare(Token &name) {
        if (scopes.empty()) return;

        std::map<std::string, bool> &scope = scopes.back();
        if (scope.find(name.lexeme) != scope.end()) {
            error(name, "Already a variable with this name in this scope.");
        }
        scope[name.lexeme] = false;
    }

    void Resolver::define(Token &name) {
        if (scopes.empty()) return;
        scopes.back()[name.lexeme] = true;
    }

    Object *Resolver::visitBlockStmt(Block *stmt) {
        beginScope();
        resolve(stmt->statements);
        endScope();
        return nullptr;
    }

    Object *Resolver::visitClassStmt(Class *stmt) {
        ClassType enclosingClass = currentClass;
        currentClass = ClassType::CLASS;

        declare(stmt->name);
        define(stmt->name);

        if (stmt->superclass != nullptr && stmt->name.lexeme == stmt->superclass->name.lexeme) {
            error(stmt->superclass->name, "A class can't inherit from itself.");
        }

        if (stmt->superclass != nullptr) {
            currentClass = ClassType::SUBCLASS;
            resolve(stmt->superclass);
        }

        if (stmt->superclass != nullptr) {
            beginScope();
            scopes.back()["super"] = true;
        }

        beginScope();
        scopes.back()["this"] = true;

        for (Function *method: stmt->methods) {
            FunctionType declaration = FunctionType::METHOD;
            if (method->name.lexeme == "init") {
                declaration = FunctionType::INITIALIZER;
            }
            resolveFunction(method, declaration);
        }

        endScope();

        if (stmt->superclass != nullptr) endScope();

        currentClass = enclosingClass;

        return nullptr;
    }

    Object *Resolver::visitExpressionStmt(Expression *stmt) {
        resolve(stmt->expression);
        return nullptr;
    }

    Object *Resolver::visitFunctionStmt(Function *stmt) {
        declare(stmt->name);
        define(stmt->name);

        resolveFunction(stmt, FunctionType::FUNCTION);
        return nullptr;
    }

    Object *Resolver::visitIfStmt(If *stmt) {
        resolve(stmt->condition);
        resolve(stmt->thenBranch);
        if (stmt->elseBranch != nullptr) resolve(stmt->elseBranch);
        return nullptr;
    }

    Object *Resolver::visitPrintStmt(Print *stmt) {
        resolve(stmt->expression);
        return nullptr;
    }

    Object *Resolver::visitReturnStmt(Return *stmt) {
        if (currentFunction == FunctionType::NONE) {
            error(stmt->keyword, "Can't return from top-level code.");
        }

        if (stmt->value != nullptr) {
            if (currentFunction == FunctionType::INITIALIZER) {
                error(stmt->keyword, "Can't return a value from an initializer.");
            }
            resolve(stmt->value);
        }
        return nullptr;
    }

    Object *Resolver::visitVarStmt(Var *stmt) {
        declare(stmt->name);
        if (stmt->initializer != nullptr) {
            resolve(stmt->initializer);
        }
        define(stmt->name);
        return nullptr;
    }

    Object *Resolver::visitWhileStmt(While *stmt) {
        resolve(stmt->condition);
        resolve(stmt->body);
        return nullptr;
    }

    Object *Resolver::visitAssignExpr(Assign *expr) {
        resolve(expr->value);
        resolveLocal(expr, expr->name);
        return nullptr;
    }

    Object *Resolver::visitBinaryExpr(Binary *expr) {
        resolve(expr->left);
        resolve(expr->right);
        return nullptr;
    }

    Object *Resolver::visitCallExpr(Call *expr) {
        resolve(expr->callee);

        for (Expr *argument: expr->arguments) {
            resolve(argument);
        }
        return nullptr;
    }

    Object *Resolver::visitGetExpr(Get *expr) {
        resolve(expr->object);
        return nullptr;
    }

    Object *Resolver::visitGroupingExpr(Grouping *expr) {
        resolve(expr->expression);
        return nullptr;
    }

    Object *Resolver::visitLiteralExpr(Literal *expr) {
        return nullptr;
    }

    Object *Resolver::visitLogicalExpr(Logical *expr) {
        resolve(expr->left);
        resolve(expr->right);
        return nullptr;
    }

    Object *Resolver::visitSetExpr(Set *expr) {
        resolve(expr->value);
        resolve(expr->object);
        return nullptr;
    }

    Object *Resolver::visitSuperExpr(Super *expr) {
        if (currentClass == ClassType::NONE) {
            error(expr->keyword, "Can't use 'super' outside of a class.");
        } else if (currentClass != ClassType::SUBCLASS) {
            error(expr->keyword, "Can't use 'super' in a class with no superclass.");
        }
        resolveLocal(expr, expr->keyword);
        return nullptr;
    }

    Object *Resolver::visitThisExpr(This *expr) {
        if (currentClass == ClassType::NONE) {
            error(expr->keyword, "Can't use 'this' outside of a class.");
            return nullptr;
        }

        resolveLocal(expr, expr->keyword);
        return nullptr;
    }

    Object *Resolver::visitUnaryExpr(Unary *expr) {
        resolve(expr->right);
        return nullptr;
    }

    Object *Resolver::visitVariableExpr(Variable *expr) {
        if (!scopes.empty() && !scopes.back()[expr->name.lexeme]) {
            error(expr->name, "Can't read local variable in its own initializer.");
        }

        resolveLocal(expr, expr->name);
        return nullptr;
    }
}