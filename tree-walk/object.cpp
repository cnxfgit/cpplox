//
// Created by hlx on 2023/9/21.
//

#include "object.h"

namespace cpplox{
    Boolean::Boolean(bool value) {
        this->value = value;
    }

    std::string Boolean::toString() {
        return value ? "true" : "false";
    }

    Object *Boolean::clone() {
        return new Boolean(this->value);
    }

    Boolean::~Boolean() = default;

    Number::Number(double value) {
        this->value = value;
    }

    std::string Number::toString() {
        return std::to_string(value);
    }

    Object *Number::clone() {
        return new Number(this->value);
    }

    Number::~Number() = default;

    String::String(std::string value) {
        this->value = value;
    }

    std::string String::toString() {
        return value;
    }

    Object *String::clone() {
        return new String(*this);
    }

    String::~String() = default;
}