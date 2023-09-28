//
// Created by hlx on 2023/9/27.
//

#include <iostream>

#include "interpreter.h"
#include "callable.h"
#include "class.h"
#include "instance.h"

namespace cpplox {

    void runtimeError(RuntimeError &error);

    Interpreter::Interpreter() {
        globals = new Environment();
        environment = globals;

        globals->define("clock",
                        new NativeFn([](Interpreter *interpreter, const std::vector<Object *> &arguments) {
                            return new Number(clock() / 1000.0);
                        }, 0));
    }

    void Interpreter::resolve(Expr *expr, int depth) {
        locals[expr] = depth;
    }


    void Interpreter::interpret(std::vector<Stmt *> &statements) {
        try {
            for (Stmt *statement: statements) {
                execute(statement);
            }
        } catch (RuntimeError &error) {
            runtimeError(error);
        }
    }

    Interpreter::~Interpreter() {
        delete globals;
    }

    void Interpreter::execute(Stmt *stmt) {
        stmt->accept(this);
    }

    void Interpreter::executeBlock(const std::vector<Stmt *> &statements, Environment *env) {
        Environment *previous = this->environment;
        try {
            this->environment = env;

            for (Stmt *statement: statements) {
                execute(statement);
            }
        } catch (ReturnException &e) {
            this->environment = previous;
            throw e;
        }
        this->environment = previous;
    }

    Object *Interpreter::visitBlockStmt(Block *stmt) {
        executeBlock(stmt->statements, new Environment(environment));
        return nullptr;
    }

    Object *Interpreter::visitClassStmt(Class *stmt) {
        Object *superclass = nullptr;
        if (stmt->superclass != nullptr) {
            superclass = evaluate(stmt->superclass);
            auto clazz = dynamic_cast<LoxClass *>(superclass);
            if (clazz == nullptr) {
                throw RuntimeError(stmt->superclass->name, "Superclass must be a class.");
            }
        }

        environment->define(stmt->name.lexeme, nullptr);

        if (stmt->superclass != nullptr) {
            environment = new Environment(environment);
            environment->define("super", superclass);
        }

        std::map<std::string, LoxFunction *> methods;
        for (Function *method: stmt->methods) {
            auto *function = new LoxFunction(method,
                                             environment, method->name.lexeme == "init");
            methods[method->name.lexeme] = function;
        }

        LoxClass *klass = new LoxClass(stmt->name.lexeme, (LoxClass *) superclass, methods);

        if (superclass != nullptr) {
            environment = environment->enclosing;
        }

        environment->assign(stmt->name, klass);
        return nullptr;
    }

    Object *Interpreter::evaluate(Expr *expr) {
        return expr->accept(this);
    }

    Object *Interpreter::visitExpressionStmt(Expression *stmt) {
        evaluate(stmt->expression);
        return nullptr;
    }

    Object *Interpreter::visitFunctionStmt(Function *stmt) {
        auto *function = new LoxFunction(stmt, environment, false);
        environment->define(stmt->name.lexeme, function);
        return nullptr;
    }

    bool isTruthy(Object *object) {
        if (object == nullptr) return false;
        auto boolean = dynamic_cast<Boolean *>(object);
        if (boolean != nullptr) return boolean->value;
        return true;
    }

    Object *Interpreter::visitIfStmt(If *stmt) {
        if (isTruthy(evaluate(stmt->condition))) {
            execute(stmt->thenBranch);
        } else if (stmt->elseBranch != nullptr) {
            execute(stmt->elseBranch);
        }
        return nullptr;
    }

    bool endsWith(const std::string &str, const std::string &suffix) {
        if (suffix.size() > str.size()) {
            return false;
        }

        return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
    }

    std::string stringify(Object *object) {
        if (object == nullptr) return "nil";

        auto *number = dynamic_cast<Number *>(object);
        if (number != nullptr) {
            std::string text = number->toString();
            if (endsWith(text, ".0")) {
                text = text.substr(0, text.length() - 2);
            }
            return text;
        }

        return object->toString();
    }

    Object *Interpreter::visitPrintStmt(Print *stmt) {
        Object *value = evaluate(stmt->expression);
        std::cout << stringify(value) << std::endl;
        return nullptr;
    }

    Object *Interpreter::visitReturnStmt(Return *stmt) {
        Object *value = nullptr;
        if (stmt->value != nullptr) value = evaluate(stmt->value);
        throw ReturnException(value);
    }

    Object *Interpreter::visitVarStmt(Var *stmt) {
        Object *value = nullptr;
        if (stmt->initializer != nullptr) {
            value = evaluate(stmt->initializer);
        }

        environment->define(stmt->name.lexeme, value);
        return nullptr;
    }

    Object *Interpreter::visitAssignExpr(Assign *expr) {
        Object *value = evaluate(expr->value);

        if (locals.find(expr) != locals.end()) {
            int distance = locals[expr];
            environment->assignAt(distance, expr->name, value);
        } else {
            globals->assign(expr->name, value);
        }

        return value;
    }

    void checkNumberOperands(Token &op, Object *left, Object *right) {
        auto le = dynamic_cast<Number *>(left);
        auto ri = dynamic_cast<Number *>(right);
        if (le != nullptr && ri != nullptr) return;
        throw RuntimeError(op, "Operands must be numbers.");
    }

    bool isEqual(Object *a, Object *b) {
        if (a == nullptr && b == nullptr) return true;
        if (a == nullptr) return false;
        return a == b;
    }

    Object *Interpreter::visitBinaryExpr(Binary *expr) {
        Object *left = evaluate(expr->left);
        Object *right = evaluate(expr->right);

        switch (expr->op.type) {
            case TokenType::GREATER:
                checkNumberOperands(expr->op, left, right);
                return new Boolean(((Number *) left)->value > ((Number *) right)->value);
            case TokenType::GREATER_EQUAL:
                checkNumberOperands(expr->op, left, right);
                return new Boolean(((Number *) left)->value >= ((Number *) right)->value);
            case TokenType::LESS:
                checkNumberOperands(expr->op, left, right);
                return new Boolean(((Number *) left)->value < ((Number *) right)->value);
            case TokenType::LESS_EQUAL:
                checkNumberOperands(expr->op, left, right);
                return new Boolean(((Number *) left)->value <= ((Number *) right)->value);
            case TokenType::MINUS:
                checkNumberOperands(expr->op, left, right);
                return new Number(((Number *) left)->value - ((Number *) right)->value);
            case TokenType::PLUS: {
                auto leNum = dynamic_cast<Number *>(left);
                auto riNum = dynamic_cast<Number *>(right);
                if (leNum != nullptr && riNum != nullptr) {
                    return new Number(((Number *) left)->value + ((Number *) right)->value);
                }
                auto leStr = dynamic_cast<String *>(left);
                auto riStr = dynamic_cast<String *>(right);
                if (leStr != nullptr && riStr != nullptr) {
                    return new String(leStr->toString() + riStr->toString());
                }
                throw RuntimeError(expr->op, "Operands must be two numbers or two strings.");
            }
            case TokenType::SLASH:
                checkNumberOperands(expr->op, left, right);
                return new Number(((Number *) left)->value / ((Number *) right)->value);
            case TokenType::STAR:
                checkNumberOperands(expr->op, left, right);
                return new Number(((Number *) left)->value * ((Number *) right)->value);
            case TokenType::BANG_EQUAL:
                return new Boolean(!isEqual(left, right));
            case TokenType::EQUAL_EQUAL:
                return new Boolean(isEqual(left, right));
        }

        return nullptr;
    }

    Object *Interpreter::visitCallExpr(Call *expr) {
        Object *callee = evaluate(expr->callee);

        std::vector<Object *> arguments;
        for (Expr *argument: expr->arguments) {
            arguments.push_back(evaluate(argument));
        }

        auto function = dynamic_cast<LoxCallable *>(callee);
        if (function == nullptr) {
            throw RuntimeError(expr->paren, "Can only call functions and classes.");
        }

        if (arguments.size() != function->arity()) {
            throw RuntimeError(expr->paren, "Expected " + std::to_string(function->arity())
                                            + " arguments but got " + std::to_string(arguments.size()) + ".");
        }

        return function->call(this, arguments);
    }

    Object *Interpreter::visitGetExpr(Get *expr) {
        Object *object = evaluate(expr->object);
        auto value = dynamic_cast<LoxInstance *>(object);
        if (value != nullptr) {
            return value->get(expr->name);
        }

        throw RuntimeError(expr->name,
                           "Only instances have properties.");
    }

    Object *Interpreter::visitGroupingExpr(Grouping *expr) {
        return evaluate(expr->expression);
    }

    Object *Interpreter::visitLiteralExpr(Literal *expr) {
        return expr->value;
    }

    Object *Interpreter::visitLogicalExpr(Logical *expr) {
        Object *left = evaluate(expr->left);

        if (expr->op.type == TokenType::OR) {
            if (isTruthy(left)) return left;
        } else {
            if (!isTruthy(left)) return left;
        }

        return evaluate(expr->right);
    }

    Object *Interpreter::visitSetExpr(Set *expr) {
        Object *object = evaluate(expr->object);

        auto instance = dynamic_cast<LoxInstance *> (object);
        if (instance == nullptr) {
            throw RuntimeError(expr->name, "Only instances have fields.");
        }

        Object *value = evaluate(expr->value);
        instance->set(expr->name, value);
        return value;
    }

    Object *Interpreter::visitSuperExpr(Super *expr) {
        int distance = locals[expr];
        LoxClass *superclass = (LoxClass *) environment->getAt(distance, "super");

        LoxInstance *object = (LoxInstance *) environment->getAt(distance - 1, "this");

        LoxFunction *method = superclass->findMethod(expr->method.lexeme);

        if (method == nullptr) {
            throw RuntimeError(expr->method,
                               "Undefined property '" + expr->method.lexeme + "'.");
        }

        return method->bind(object);
    }

    Object *Interpreter::lookUpVariable(Token &name, Expr *expr) {
        if (locals.find(expr) != locals.end()) {
            int distance = locals[expr];
            return environment->getAt(distance, name.lexeme);
        } else {
            return globals->get(name);
        }
    }

    Object *Interpreter::visitThisExpr(This *expr) {
        return lookUpVariable(expr->keyword, expr);
    }

    void checkNumberOperand(Token &op, Object *operand) {
        if (dynamic_cast<Number *>(operand) != nullptr) return;
        throw RuntimeError(op, "Operand must be a number.");
    }

    Object *Interpreter::visitUnaryExpr(Unary *expr) {
        Object *right = evaluate(expr->right);

        switch (expr->op.type) {
            case TokenType::BANG:
                return new Boolean(!isTruthy(right));
            case TokenType::MINUS:
                checkNumberOperand(expr->op, right);
                return new Number(-((Number *) right)->value);
        }

        return nullptr;
    }

    Object *Interpreter::visitVariableExpr(Variable *expr) {
        return lookUpVariable(expr->name, expr);
    }

    Object *Interpreter::visitWhileStmt(While *stmt) {
        while (isTruthy(evaluate(stmt->condition))) {
            execute(stmt->body);
        }
        return nullptr;
    }

}