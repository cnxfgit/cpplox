//
// Created by hlx on 2023/9/28.
//

#ifndef CPPLOX_CLASS_H
#define CPPLOX_CLASS_H

#include <map>
#include "object.h"
#include "function.h"

namespace cpplox {

    class LoxClass : public LoxCallable {
    public:
        std::string name;
        LoxClass *superclass;
        std::map<std::string, LoxFunction*> methods;

        LoxClass(std::string name, LoxClass *superclass, std::map<std::string, LoxFunction*> methods);

        Object * clone() override;

        std::string toString() override;

        int arity() override;

        Object * call(Interpreter *interpreter, std::vector<Object *> arguments) override;

        LoxFunction* findMethod(std::string name);

        ~LoxClass() override = default;
    };

}

#endif //CPPLOX_CLASS_H
