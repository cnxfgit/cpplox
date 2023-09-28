//
// Created by hlx on 2023/9/28.
//

#ifndef CPPLOX_FUNCTION_H
#define CPPLOX_FUNCTION_H

#include "stmt.h"
#include "environment.h"
#include "callable.h"

namespace cpplox {

    class LoxInstance;

    class LoxFunction : public LoxCallable {
    private:
        Function *declaration;
        Environment *closure;
        bool isInitializer;
    public:
        LoxFunction(Function *declaration, Environment *closure, bool isInitializer);

        std::string toString() override;

        LoxFunction *bind(LoxInstance* instance);

        Object *clone() override;

        int arity() override;

        Object * call(Interpreter *interpreter, std::vector<Object *> arguments) override;

        ~LoxFunction() override = default;
    };

}

#endif //CPPLOX_FUNCTION_H
