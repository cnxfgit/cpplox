//
// Created by hlx on 2023/9/28.
//

#include "instance.h"

namespace cpplox {

    Object *LoxInstance::clone() {
        return nullptr;
    }

    std::string LoxInstance::toString() {
        return klass->name + " instance";
    }

    LoxInstance::LoxInstance(LoxClass *klass) {
        this->klass = klass;
    }

    Object *LoxInstance::get(Token &name) {
        if (fields.find(name.lexeme) != fields.end()) {
            return fields[name.lexeme];
        }

        LoxFunction *method = klass->findMethod(name.lexeme);
        if (method != nullptr) return method->bind(this);

        throw RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
    }

    void LoxInstance::set(Token &name, Object *value) {
        fields[name.lexeme] = value;
    }
}