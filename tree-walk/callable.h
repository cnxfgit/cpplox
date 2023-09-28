//
// Created by hlx on 2023/9/28.
//

#ifndef CPPLOX_CALLABLE_H
#define CPPLOX_CALLABLE_H

#include "object.h"
#include "interpreter.h"
#include <functional>
#include <utility>

namespace cpplox {
    class LoxCallable : public Object {
    public:

        std::string toString() override = 0;

        Object *clone() override = 0;

        virtual int arity() = 0;

        virtual Object *call(Interpreter *interpreter, std::vector<Object *> arguments) = 0;

        ~LoxCallable() override = default;
    };

    class NativeFn : public LoxCallable {
    private:
        std::function<Object *(Interpreter *, std::vector<Object *>)> fn;
        int _arity = 0;
    public:
        NativeFn(std::function<Object *(Interpreter *, std::vector<Object *>)> fn, int arity) {
            this->fn = std::move(fn);
            this->_arity = arity;
        }

        std::string toString() override;

        Object *clone() override;

        int arity() override;

        Object *call(Interpreter *interpreter, std::vector<Object *> arguments) override;

        ~NativeFn() override = default;
    };
}


#endif //CPPLOX_CALLABLE_H
