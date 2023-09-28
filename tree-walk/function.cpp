//
// Created by hlx on 2023/9/28.
//

#include "function.h"
#include "instance.h"

namespace cpplox {

    LoxFunction::LoxFunction(Function *declaration, Environment *closure, bool isInitializer) {
        this->declaration = declaration;
        this->closure = closure;
        this->isInitializer = isInitializer;
    }

    std::string LoxFunction::toString() {
        return "<fn " + declaration->name.lexeme + ">";
    }

    Object *LoxFunction::clone() {
        return nullptr;
    }

    int LoxFunction::arity() {
        return (int) this->declaration->params->size();
    }

    Object *LoxFunction::call(Interpreter *interpreter, std::vector<Object *> arguments) {
        auto *environment = new Environment(closure);
        for (int i = 0; i < declaration->params->size(); i++) {
            auto params = declaration->params;
            environment->define((*params)[i].lexeme, arguments[i]);
        }

        try {
            interpreter->executeBlock(declaration->body, environment);
        } catch (ReturnException &returnValue) {
            return returnValue.value;
        }

        if (isInitializer) return closure->getAt(0, "this");
        return nullptr;
    }

    LoxFunction *LoxFunction::bind(LoxInstance *instance) {
        auto environment = new Environment(closure);
        environment->define("this", instance);
        return new LoxFunction(declaration, environment, isInitializer);
    }
}