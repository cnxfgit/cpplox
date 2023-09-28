//
// Created by hlx on 2023/9/28.
//

#ifndef CPPLOX_INSTANCE_H
#define CPPLOX_INSTANCE_H

#include "stmt.h"
#include "class.h"

namespace cpplox {
    class LoxInstance : public Object {
    public:
        LoxClass* klass;
        std::map<std::string, Object *> fields;

        explicit LoxInstance(LoxClass *klass);

        Object* get(Token& name);

        void set(Token& name, Object* value);

        Object *clone() override;

        std::string toString() override;

        ~LoxInstance() override = default;
    };
}

#endif //CPPLOX_INSTANCE_H
