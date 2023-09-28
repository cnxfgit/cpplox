//
// Created by hlx on 2023/9/28.
//

#include "callable.h"

namespace cpplox{
    std::string NativeFn::toString() {
        return "<native fn>";
    }

    Object *NativeFn::clone() {
        return new NativeFn(this->fn, this->_arity);
    }

    int NativeFn::arity() {
        return this->_arity;
    }

    Object *NativeFn::call(Interpreter *interpreter, std::vector<Object *> arguments) {
        return this->fn(interpreter, arguments);
    }


}




