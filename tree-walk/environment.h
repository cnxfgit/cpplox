//
// Created by hlx on 2023/9/28.
//

#ifndef CPPLOX_ENVIRONMENT_H
#define CPPLOX_ENVIRONMENT_H

#include <map>
#include <string>
#include "object.h"
#include "token.h"

namespace cpplox {
    class Environment {
    private:
        std::map<std::string, Object *> values;
    public:
        Environment *enclosing;

        explicit Environment(Environment *enclosing);

        Environment();

        Object *get(Token &name);

        void assign(Token &name, Object *value);

        void define(std::string name, Object *value);

        Environment* ancestor(int distance);

        Object* getAt(int distance, const std::string& name);

        void assignAt(int distance, Token& name, Object* value);

        ~Environment();
    };
}

#endif //CPPLOX_ENVIRONMENT_H
