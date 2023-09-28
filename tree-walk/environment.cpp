//
// Created by hlx on 2023/9/28.
//

#include "environment.h"
#include "interpreter.h"

namespace cpplox {

    Environment::Environment() {
        this->enclosing = nullptr;
    }

    Environment::Environment(Environment *enclosing) {
        this->enclosing = enclosing;
    }

    Object *Environment::get(Token &name) {
        if (values.find(name.lexeme) != values.end()) {
            return values[name.lexeme];
        }

        if (enclosing != nullptr) return enclosing->get(name);

        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    void Environment::assign(Token &name, Object *value) {
        if (values.find(name.lexeme) != values.end()) {
            values[name.lexeme] = value;
            return;
        }

        if (enclosing != nullptr) {
            enclosing->assign(name, value);
            return;
        }

        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    void Environment::define(std::string name, Object *value) {
        values[name] = value;
    }

    Environment *Environment::ancestor(int distance) {
        Environment *environment = this;
        for (int i = 0; i < distance; i++) {
            environment = environment->enclosing;
        }
        return environment;
    }

    Object *Environment::getAt(int distance, const std::string &name) {
        return ancestor(distance)->values[name];
    }

    void Environment::assignAt(int distance, Token &name, Object *value) {
        ancestor(distance)->values[name.lexeme] = value;
    }


    Environment::~Environment() = default;
}