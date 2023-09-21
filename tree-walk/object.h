//
// Created by hlx on 2023/9/21.
//

#ifndef CPPLOX_OBJECT_H
#define CPPLOX_OBJECT_H

#include <string>

namespace cpplox{
    class Object {
    public:
        virtual std::string toString() = 0;

        virtual ~Object() = default;
    };

    class Boolean : public Object {
        bool value;
    public:
        explicit Boolean(bool value);

        std::string toString() override;

        ~Boolean() override;
    };

    class Number : public Object {
        double value;
    public:
        explicit Number(double value);

        std::string toString() override;

        ~Number() override;
    };

    class String : public Object {
        std::string value;
    public:
        explicit String(std::string value);

        std::string toString() override;

        ~String() override;
    };
}

#endif //CPPLOX_OBJECT_H