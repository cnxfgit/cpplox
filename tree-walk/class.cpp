//
// Created by hlx on 2023/9/28.
//

#include "class.h"
#include "instance.h"
#include <utility>

namespace cpplox {

    std::string LoxClass::toString() {
        return this->name;
    }

    LoxClass::LoxClass(std::string name, LoxClass *superclass, std::map<std::string, LoxFunction*> methods) {
        this->superclass = superclass;
        this->name = std::move(name);
        this->methods = std::move(methods);
    }

    Object *LoxClass::clone() {
        return nullptr;
    }

    int LoxClass::arity() {
        LoxFunction *initializer = findMethod("init");
        if (initializer == nullptr) return 0;
        return initializer->arity();
    }

    Object *LoxClass::call(Interpreter *interpreter, std::vector<Object *> arguments) {
        LoxInstance *instance = new LoxInstance(this);
        LoxFunction *initializer = findMethod("init");
        if (initializer != nullptr) {
            initializer->bind(instance)->call(interpreter, arguments);
        }
        return instance;
    }

    LoxFunction *LoxClass::findMethod(std::string name) {
        if (methods.find(name) != methods.end()) {
            return methods[name];
        }

        if (superclass != nullptr) {
            return superclass->findMethod(name);
        }
        return nullptr;
    }
}